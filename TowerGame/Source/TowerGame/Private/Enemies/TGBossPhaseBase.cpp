// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/TGBossPhaseBase.h"

#include "Enemies/TGBossBase.h"
#include "Enemies/TGPatternBase.h"
#include "Player/TGPlayer.h"

UTGBossPhaseBase::UTGBossPhaseBase() :
	OwnerBoss(nullptr),
	CurrentPattern(nullptr),
	bIsTransitionPatternRunning(false),
	WarningDrawTime(1.5f),
	PatternInterval(4.f)
{
}

void UTGBossPhaseBase::Initialize(ATGBossBase* InOwnerBoss)
{
	OwnerBoss = InOwnerBoss;

	CreatePatterns();
}

void UTGBossPhaseBase::CreatePatterns()
{
	Patterns.Empty();
	LastPatternStartTimes.Empty();

	if (!OwnerBoss) return;

	// pattern 생성하여 저장
	for (const FTGBossPatternEntry& Entry: PatternEntries){
		if (!Entry.PatternClass){
			Patterns.Add(nullptr);
			LastPatternStartTimes.Add(-TNumericLimits<float>::Max());
			continue;
		}

		UTGPatternBase* Pattern = NewObject<UTGPatternBase>(this, Entry.PatternClass);
		if (!Pattern){
			Patterns.Add(nullptr);
			LastPatternStartTimes.Add(-TNumericLimits<float>::Max());
			continue;
		}

		Pattern->Initialize(
			this,
			OwnerBoss,
			Entry.WarningRadius,
			Entry.AttackDamage,
			Entry.AttackEffectScale
		);

		Patterns.Add(Pattern);
		LastPatternStartTimes.Add(-TNumericLimits<float>::Max());
	}
}

bool UTGBossPhaseBase::CanUsePatternEntry(int32 PatternIndex) const
{
	// Pattern이 유효하지 않은 index일 경우
	if (!PatternEntries.IsValidIndex(PatternIndex)) return false;
	if (!Patterns.IsValidIndex(PatternIndex) || !Patterns[PatternIndex]) return false;
	if (!LastPatternStartTimes.IsValidIndex(PatternIndex)) return false;

	// Pattern Cooldown 이 없는 경우
	const FTGBossPatternEntry& Entry = PatternEntries[PatternIndex];

	// 거리 검사
	if (!IsPlayerDistanceMatched(Entry)) return false;
	if (Entry.PatternCooldown <= 0.f) return true;

	UWorld* World = OwnerBoss->GetWorld();
	if (!World) return false;

	// Pattern의 Cooldown 확인
	return World->GetTimeSeconds() - LastPatternStartTimes[PatternIndex] >= Entry.PatternCooldown;
}

bool UTGBossPhaseBase::IsPlayerDistanceMatched(const FTGBossPatternEntry& Entry) const
{
	// 거리 제한 X
	if (Entry.MinPlayerDistance <= 0.f && Entry.MaxPlayerDistance <= 0.f) return true;

	if (!OwnerBoss) return false;

	ATGPlayer* Player = OwnerBoss->GetPlayer();
	if (!Player) return false;

	const float PlayerDistance = FVector::Dist2D(OwnerBoss->GetActorLocation(), Player->GetActorLocation());

	// MinDistance 안일 경우
	if (Entry.MinPlayerDistance > 0.f && PlayerDistance < Entry.MinPlayerDistance) return false;
	// Maxdistance 밖일 경우
	if (Entry.MaxPlayerDistance > 0.f && PlayerDistance > Entry.MaxPlayerDistance) return false;

	return true;
}

bool UTGBossPhaseBase::IsPhaseTransitionConditionMatched() const
{
	if (!OwnerBoss) return false;

	return OwnerBoss->GetCurrentHP() > 0.f
		&& OwnerBoss->GetCurrentHP() <= OwnerBoss->GetCurrentPhaseMinHP();
}

void UTGBossPhaseBase::EnterPhase()
{
	UE_LOG(LogTemp, Log, TEXT("%s EnterPhase"), *GetName());

	if (!OwnerBoss) return;

	TArray<FName> BreakablePartTags;

	// 공격 가능 파츠 태그
	for (FTGBossBreakablePartData& Part : BreakableParts){
		if (Part.PartTag == NAME_None || Part.HPRatio <= 0.f){
			Part.CurrentHP = 0.f;
			continue;
		}

		Part.CurrentHP = Part.HPRatio * OwnerBoss->GetMaxHP();
		BreakablePartTags.Add(Part.PartTag);
	}

	OwnerBoss->SetActiveBreakablePartTags(BreakablePartTags);

	UWorld* World = OwnerBoss->GetWorld();
	if (!World) return;

	// 패턴 공격 시작
	World->GetTimerManager().ClearTimer(PatternTimerHandle);
	World->GetTimerManager().SetTimer(
		PatternTimerHandle,
		this,
		&UTGBossPhaseBase::ExecutePattern,
		PatternInterval,
		false,
		PatternInterval
	);
}

void UTGBossPhaseBase::ExitPhase()
{
	// Phase 전환 시 이전 Phase의 반복 패턴 호출을 중단한다.
	if (OwnerBoss){
		if (UWorld* World = OwnerBoss->GetWorld()){
			World->GetTimerManager().ClearTimer(PatternTimerHandle);
		}
	}

	// Pattern Timer 정리
	for (UTGPatternBase* Pattern : Patterns){
		if (Pattern){
			Pattern->OnPatternFinished.RemoveAll(this);
			Pattern->StopPattern();
		}
	}

	CurrentPattern = nullptr;
	bIsTransitionPatternRunning = false;
}

void UTGBossPhaseBase::ExecutePattern()
{
	if (!OwnerBoss || Patterns.IsEmpty()) return;

	UTGPatternBase* SelectedPattern = nullptr;
	int32 SelectedPatternIndex = INDEX_NONE;
	// 현재 페이즈의 체력 0
	bIsTransitionPatternRunning =
		OwnerBoss->GetCurrentHP() > 0.f &&
		OwnerBoss->GetCurrentHP() <= OwnerBoss->GetCurrentPhaseMinHP();

	// Phase 전환 조건일 경우 마지막 Pattern을 실행
	if (bIsTransitionPatternRunning){
		const int32 TransitionPatternIndex = Patterns.Num() - 1;

		if (Patterns.IsValidIndex(TransitionPatternIndex) && Patterns[TransitionPatternIndex]){
			SelectedPattern = Patterns[TransitionPatternIndex];
			SelectedPatternIndex = TransitionPatternIndex;
		}
	}

	// 사용할 패턴 선택
	for (int32 PatternIndex = 0; PatternIndex < Patterns.Num(); ++PatternIndex){
		if (SelectedPattern) break;
		if (!CanUsePatternEntry(PatternIndex)) continue;

		SelectedPattern = Patterns[PatternIndex];
		SelectedPatternIndex = PatternIndex;
		break;
	}

	if (!SelectedPattern || SelectedPatternIndex == INDEX_NONE) return;

	if (CurrentPattern){
		CurrentPattern->OnPatternFinished.RemoveAll(this);
		CurrentPattern->StopPattern();
	}

	UWorld* World = OwnerBoss->GetWorld();
	if (!World) return;

	// 패턴 시작 시점 저장
	LastPatternStartTimes[SelectedPatternIndex] = World->GetTimeSeconds();

	CurrentPattern = SelectedPattern;
	CurrentPattern->StartPattern(WarningDrawTime);
	CurrentPattern->OnPatternFinished.AddUObject(this, &UTGBossPhaseBase::HandlePatternFinished);
}

void UTGBossPhaseBase::HandlePatternFinished()
{
	if (!OwnerBoss) return;

	if (CurrentPattern){
		CurrentPattern->OnPatternFinished.RemoveAll(this);
	}

	UWorld* World = OwnerBoss->GetWorld();
	if (!World) return;

	// 페이즈 전환 패턴이었는지 확인
	if (bIsTransitionPatternRunning){
		bIsTransitionPatternRunning = false;
		OnPhaseTransitionReady.Broadcast();
		return;
	}

	// 패턴 종료 뒤 1초 뒤 다음 패턴 실행
	World->GetTimerManager().ClearTimer(PatternTimerHandle);
	World->GetTimerManager().SetTimer(
		PatternTimerHandle,
		this,
		&UTGBossPhaseBase::ExecutePattern,
		1.f,
		false
	);
}

FTGBossBreakablePartData* UTGBossPhaseBase::FindBreakablePart(FName PartTag)
{
	// BreakableParts에서 Tag 검색
	for (FTGBossBreakablePartData& Part : BreakableParts){
		if (Part.PartTag == PartTag){
			return &Part;
		}
	}

	return nullptr;
}

void UTGBossPhaseBase::RemoveBreakablePart(FName PartTag)
{
	BreakableParts.RemoveAll([PartTag](const FTGBossBreakablePartData& Part)
	{
		return Part.PartTag == PartTag;
	});
}
