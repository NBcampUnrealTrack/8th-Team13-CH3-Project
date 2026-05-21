// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/TGBossPhaseBase.h"

#include "Enemies/TGBossBase.h"
#include "Enemies/TGPatternBase.h"

UTGBossPhaseBase::UTGBossPhaseBase() :
	OwnerBoss(nullptr),
	CurrentPattern(nullptr),
	WarningDrawTime(1.5f),
	PatternInterval(3.f)
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

	if (!OwnerBoss) return;

	// pattern 생성하여 저장
	for (const FTGBossPatternEntry& Entry: PatternEntries){
		if (!Entry.PatternClass) continue;

		UTGPatternBase* Pattern = NewObject<UTGPatternBase>(this, Entry.PatternClass);
		if (!Pattern) continue;

		Pattern->Initialize(
			this,
			OwnerBoss,
			Entry.WarningRadius,
			Entry.AttackDamage,
			Entry.AttackEffectScale
		);

		Patterns.Add(Pattern);
	}
}

void UTGBossPhaseBase::ExecuteDelayedAttack()
{
	if (!CurrentPattern) return;

	CurrentPattern->ExecuteAttack();
	CurrentPattern = nullptr;
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
		true,
		PatternInterval
	);
}

void UTGBossPhaseBase::ExitPhase()
{
	// Phase 전환 시 이전 Phase의 반복 패턴 호출을 중단한다.
	if (OwnerBoss){
		if (UWorld* World = OwnerBoss->GetWorld()){
			World->GetTimerManager().ClearTimer(PatternTimerHandle);
			World->GetTimerManager().ClearTimer(AttackDelayTimerHandle);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("%s ExitPhase"), *GetName());
}

void UTGBossPhaseBase::ExecutePattern()
{
	if (!OwnerBoss || Patterns.IsEmpty()) return;

	UWorld* World = OwnerBoss->GetWorld();
	if (!World) return;

	// 임시로 현재 패턴을 첫번째 패턴으로 고정
	CurrentPattern = Patterns[0];

	if (!CurrentPattern) return;

	// 공격 위치 전달 및 공격 범위 표시
	CurrentPattern->GetAttackLocation();
	CurrentPattern->DrawWarning(WarningDrawTime);

	World->GetTimerManager().ClearTimer(AttackDelayTimerHandle);
	World->GetTimerManager().SetTimer(
		AttackDelayTimerHandle,
		this,
		&UTGBossPhaseBase::ExecuteDelayedAttack,
		WarningDrawTime,
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
