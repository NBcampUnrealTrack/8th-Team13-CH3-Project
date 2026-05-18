// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/TGBossBase.h"

#include "TGMountedTower.h"
#include "Core/GameFlow/TGGameMode.h"
#include "Enemies/TGBossPhaseBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/TGPlayer.h"

ATGBossBase::ATGBossBase() :
	MaxHP(1.f),
	CurrentHP(0.f),
	TowerDamageMultiplier(0.1f),
	SpawnClearRadius(0),
	CurrentPhase(nullptr),
	CurrentPhaseIndex(INDEX_NONE),
	TargetPlayer(nullptr)
{
	PrimaryActorTick.bCanEverTick = false;

	BossName = FText::FromString("Boss");

}

void ATGBossBase::BeginPlay()
{
	Super::BeginPlay();

	CurrentHP = MaxHP;
	OnBossHpChanged.Broadcast(CurrentHP, MaxHP);
	TargetPlayer = Cast<ATGPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));

	ChangeToNextPhase();
}

void ATGBossBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
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

	// 데미지 적용 후 페이즈 전환 검사
	ApplyBossDamage(AppliedDamage);
	CheckPhaseTransition();

	return AppliedDamage;
}

FText ATGBossBase::GetBossName() const
{
	return BossName;
}

float ATGBossBase::GetCurrentHP() const
{
	return CurrentHP;
}

float ATGBossBase::GetMaxHP() const
{
	return MaxHP;
}

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

float ATGBossBase::GetSpawnClearRadius() const
{
	return SpawnClearRadius;
}

ATGPlayer* ATGBossBase::GetPlayer() const
{
	return TargetPlayer;
}

void ATGBossBase::ChangeToNextPhase()
{
	const int32 NewPhaseIndex = CurrentPhaseIndex + 1;
	if (!PhaseClasses.IsValidIndex(NewPhaseIndex)) return;

	TSubclassOf<UTGBossPhaseBase> NewPhaseClass = PhaseClasses[NewPhaseIndex];
	if (!NewPhaseClass) return;

	// 기존 페이즈 종료
	if (CurrentPhase){
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

	UE_LOG(LogTemp, Warning, TEXT("[BossBase]BossDamaged - Damage: %.1f / HP: %.1f / %.1f "),
		DamageAmount, CurrentHP, MaxHP);

	if (CurrentHP <= 0){
		if (ATGGameMode* GameMode = Cast<ATGGameMode>(UGameplayStatics::GetGameMode(this))){
			GameMode->HandleGameClear();
		}
		Destroy();
	}
}
