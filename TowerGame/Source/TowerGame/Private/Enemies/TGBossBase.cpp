// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/TGBossBase.h"

#include "TGMountedTower.h"
#include "Core/GameFlow/TGGameMode.h"
#include "Enemies/TGBossPhaseBase.h"
#include "Enemies/TGPartBreakComponent.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Player/TGPlayer.h"

ATGBossBase::ATGBossBase() :
	MaxHP(1.f),
	CurrentHP(0.f),
	TowerDamageMultiplier(0.1f),
	SpawnClearRadius(0),
	CurrentPhase(nullptr),
	CurrentPhaseIndex(INDEX_NONE),
	bIsPatternYawRotating(false),
	PatternYawRotationSpeed(0.f),
	DeathSound(nullptr),
	TargetPlayer(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;

	BossName = FText::FromString("Boss");
	PartBreakComponent = CreateDefaultSubobject<UTGPartBreakComponent>(TEXT("PartBreakComponent"));
}

void ATGBossBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 패턴으로 인한 회전
	if (bIsPatternYawRotating){
		AddActorWorldRotation(FRotator(0.f, PatternYawRotationSpeed * DeltaTime, 0.f));
		return;
	}

	// 플레이어 방향 주시
	FocusTargetPlayer();
}

void ATGBossBase::BeginPlay()
{
	Super::BeginPlay();

	CurrentHP = MaxHP;
	OnBossHpChanged.Broadcast(CurrentHP, MaxHP);
	TargetPlayer = Cast<ATGPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));

	// Initialize & 부위 제거 델리게이트 등록
	if (PartBreakComponent){
		PartBreakComponent->Initialize(this);
		PartBreakComponent->OnDetachedPartRemoved.AddUObject(
			this,
			&ATGBossBase::HandleDetachedPartRemoved
		);
	}

	ChangeToNextPhase();
}

void ATGBossBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 부위 제거 델리게이트 제거
	if (PartBreakComponent){
		PartBreakComponent->OnDetachedPartRemoved.RemoveAll(this);
	}

	if (CurrentPhase){
		CurrentPhase->ExitPhase();
		CurrentPhase = nullptr;
	}
	OnBossRemoved.Broadcast(this);

	Super::EndPlay(EndPlayReason);
}

float ATGBossBase::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	float AppliedDamage =  Super::TakeDamage(
		DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// Tower 데미지는 보정 후 적용
	if (Cast<ATGMountedTower>(DamageCauser)){
		AppliedDamage *= TowerDamageMultiplier;
	}

	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID)){
		const FPointDamageEvent& PointDamageEvent = static_cast<const FPointDamageEvent&>(DamageEvent);

		const float PartDamageResult =
			ApplyBreakablePartDamage(PointDamageEvent.HitInfo.GetComponent(), AppliedDamage);

		if (PartDamageResult >= 0.f){
			AppliedDamage = PartDamageResult;
		}
	}

	// 데미지 적용 후 페이즈 전환 검사
	ApplyBossDamage(AppliedDamage);
	CheckPhaseTransition();

	return AppliedDamage;
}

FText ATGBossBase::GetBossName() const { return BossName; }

float ATGBossBase::GetCurrentHP() const { return CurrentHP; }

float ATGBossBase::GetMaxHP() const { return MaxHP; }

float ATGBossBase::GetCurrentPhaseMinHP() const
{
	if (!PhaseHPRatio.IsValidIndex(CurrentPhaseIndex)) return 0.f;

	return PhaseHPRatio[CurrentPhaseIndex] * MaxHP;
}

float ATGBossBase::GetCurrentPhaseMaxHP() const
{
	if (CurrentPhaseIndex <= 0) return MaxHP;

	const int32 PreviousPhaseIndex = CurrentPhaseIndex - 1;
	if (!PhaseHPRatio.IsValidIndex(PreviousPhaseIndex)) return MaxHP;

	return PhaseHPRatio[PreviousPhaseIndex] * MaxHP;
}

float ATGBossBase::GetSpawnClearRadius() const { return SpawnClearRadius; }

ATGPlayer* ATGBossBase::GetPlayer() const { return TargetPlayer; }

void ATGBossBase::SetActiveBreakablePartTags(const TArray<FName>& InBreakablePartTags)
{
	ActiveBreakablePartTags.Empty();

	// Set에 Tag 추가 공격 가능 파츠 탐색용
	for (const FName& PartTag : InBreakablePartTags){
		if (PartTag != NAME_None){
			ActiveBreakablePartTags.Add(PartTag);
		}
	}
}

void ATGBossBase::StartPatternYawRotation(float InYawRotationSpeed)
{
	PatternYawRotationSpeed = InYawRotationSpeed;
	bIsPatternYawRotating = !FMath::IsNearlyZero(PatternYawRotationSpeed);
}

void ATGBossBase::StopPatternYawRotation()
{
	bIsPatternYawRotating = false;
	PatternYawRotationSpeed = 0.f;
}

void ATGBossBase::FocusTargetPlayer()
{
	if (!TargetPlayer) return;

	// Player 방향 계산
	FVector DirectionToPlayer = TargetPlayer->GetActorLocation() - GetActorLocation();
	DirectionToPlayer.Z = 0.f;

	if (!DirectionToPlayer.Normalize()) return;

	const FRotator CurrentRotation = GetActorRotation();
	const FRotator TargetRotation = DirectionToPlayer.Rotation();

	//Yaw값만 적용
	SetActorRotation(FRotator(CurrentRotation.Pitch, TargetRotation.Yaw, CurrentRotation.Roll));
}

void ATGBossBase::ChangeToNextPhase()
{
	const int32 NewPhaseIndex = CurrentPhaseIndex + 1;
	if (!PhaseClasses.IsValidIndex(NewPhaseIndex)) return;

	TSubclassOf<UTGBossPhaseBase> NewPhaseClass = PhaseClasses[NewPhaseIndex];
	if (!NewPhaseClass) return;

	// 기존 페이즈 종료
	if (CurrentPhase){
		DestroyRemainingBreakableParts();

		CurrentPhase->ExitPhase();
		CurrentPhase = nullptr;
	}

	// 페이즈 생성
	UTGBossPhaseBase* NewPhase = NewObject<UTGBossPhaseBase>(this, NewPhaseClass);
	if (!NewPhase) return;

	// Phase에 Boss 참조
	NewPhase->Initialize(this);
	CurrentPhase = NewPhase;
	CurrentPhaseIndex = NewPhaseIndex;

	CurrentPhase->EnterPhase();
	// 제거되지 않은 부위 제거
	if (PartBreakComponent){
		PartBreakComponent->RebuildPartDamageMaterialCache(ActiveBreakablePartTags);
	}

	// 페이즈 변경 완료 시 UI에 반영
	OnBossHpChanged.Broadcast(CurrentHP, MaxHP);
}

void ATGBossBase::CheckPhaseTransition()
{
	if (GetWorldTimerManager().IsTimerActive(PhaseTransitionTimerHandle)) return;

	const int32 NextPhaseIndex = CurrentPhaseIndex + 1;
	if (!PhaseClasses.IsValidIndex(NextPhaseIndex) || !PhaseHPRatio.IsValidIndex(CurrentPhaseIndex)) return;

	// HP 비율이 현재 페이즈의 전환 기준 이하로 내려가면 다음 페이즈로 전환
	const float HPRatio = CurrentHP / MaxHP;
	if (HPRatio > PhaseHPRatio[CurrentPhaseIndex]) return;

	GetWorldTimerManager().SetTimer(
		PhaseTransitionTimerHandle, this, &ATGBossBase::ChangeToNextPhase, 1, false);
}

void ATGBossBase::ApplyBossDamage(float DamageAmount)
{
	if (DamageAmount <= 0.f || CurrentHP <= 0.f) return;

	float MinHpRatio = 0.f;
	if (PhaseHPRatio.IsValidIndex(CurrentPhaseIndex)) MinHpRatio = PhaseHPRatio[CurrentPhaseIndex];

	CurrentHP = FMath::Clamp(CurrentHP - DamageAmount, MinHpRatio * MaxHP, MaxHP);
	OnBossHpChanged.Broadcast(CurrentHP, MaxHP);

	if (CurrentHP <= 0){
		if (ATGGameMode* GameMode = Cast<ATGGameMode>(UGameplayStatics::GetGameMode(this))){
			GameMode->HandleGameOver();
		}

		if (DeathSound){
			UGameplayStatics::PlaySoundAtLocation(
				this,
				DeathSound,
				GetActorLocation(),
				1.f
			);
		}

		if (DeathEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				this,
				DeathEffect,
				GetActorLocation(),
				FRotator::ZeroRotator,
				FVector::OneVector * DeathEffectScale
			);
		}

		Destroy();
	}
}

float ATGBossBase::ApplyBreakablePartDamage(UActorComponent* HitComponent, float DamageAmount)
{
	if (!HitComponent || !CurrentPhase || DamageAmount <= 0.f) return -1.f;

	for (const FName& ComponentTag : HitComponent->ComponentTags){
		if (!ActiveBreakablePartTags.Contains(ComponentTag)) continue;

		// 해당 페이즈에 해당 파츠가 존재하는지 확인
		FTGBossBreakablePartData* PartData = CurrentPhase->FindBreakablePart(ComponentTag);
		if (!PartData){
			ActiveBreakablePartTags.Remove(ComponentTag);
			return -1.f;
		}

		// Tag 존재 + 체력 0 -> 파괴 중인 파츠
		if (PartData->CurrentHP <= 0){
			return 0.f;
		}

		// 파츠에 체력에 데미지를 보정해서 적용
		float AppliedPartDamage = FMath::Clamp(DamageAmount, 0.f, PartData->CurrentHP);
		PartData->CurrentHP -= AppliedPartDamage;

		const float MaxPartHp = PartData->HPRatio* MaxHP;
		// 다이나믹 머터리얼 값 적용
		if (PartBreakComponent){
			PartBreakComponent->UpdateBreakablePartDamageVisual(ComponentTag, PartData->CurrentHP, MaxPartHp);
		}

		// 파츠의 체력이 0일 경우 파괴 추가 데미지 적용
		if (PartData->CurrentHP <= 0){
			AppliedPartDamage += MaxHP * PartData->DestroyBonusDamageRatio;

			// 부위 파괴
			if (PartBreakComponent){
				PartBreakComponent->BreakPartsByTag(ComponentTag);
			}
		}

		return AppliedPartDamage;
	}
	return -1.f;
}

void ATGBossBase::DestroyRemainingBreakableParts()
{
	if (!CurrentPhase) return;

	// Set -> Array
	TArray<FName> PartTags = ActiveBreakablePartTags.Array();

	for (const FName& PartTag : PartTags){
		FTGBossBreakablePartData* PartData = CurrentPhase->FindBreakablePart(PartTag);
		// PartData가 없거나 현재 체력이 없는 얘들은 이미 부위 파괴 처리 중인것으로 간주
		if (!PartData || PartData->CurrentHP <= 0.f) continue;

		// 부위 파괴
		if (PartBreakComponent){
			PartBreakComponent->BreakPartsByTag(PartTag);
		}
	}
}

void ATGBossBase::HandleDetachedPartRemoved(FName PartTag)
{
	ActiveBreakablePartTags.Remove(PartTag);
	if (CurrentPhase){
		CurrentPhase->RemoveBreakablePart(PartTag);
	}
}
