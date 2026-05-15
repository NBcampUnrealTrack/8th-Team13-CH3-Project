// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/TGBossBase.h"

#include "Enemies/TGBossPhaseBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/TGPlayer.h"

ATGBossBase::ATGBossBase() :
	MaxHP(1.f),
	CurrentHP(0.f),
	CurrentPhase(nullptr),
	CurrentPhaseIndex(INDEX_NONE),
	TargetPlayer(nullptr)
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATGBossBase::BeginPlay()
{
	Super::BeginPlay();

	CurrentHP = MaxHP;
	TargetPlayer = Cast<ATGPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));

	ChangePhase(0);
}

void ATGBossBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (CurrentPhase){
		CurrentPhase->ExitPhase();
		CurrentPhase = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

float ATGBossBase::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	const float AppliedDamage =  Super::TakeDamage(
		DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 데미지 적용 후 페이즈 전환 검사
	ApplyBossDamage(AppliedDamage);
	CheckPhaseTransition();

	return AppliedDamage;
}

float ATGBossBase::GetCurrentHP() const
{
	return CurrentHP;
}

float ATGBossBase::GetMaxHP() const
{
	return MaxHP;
}

ATGPlayer* ATGBossBase::GetPlayer() const
{
	return TargetPlayer;
}

void ATGBossBase::ChangePhase(int32 NewPhaseIndex)
{
	if (CurrentPhaseIndex == NewPhaseIndex) return;
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
	const int32 NextPhaseIndex = CurrentPhaseIndex + 1;
	if (!PhaseClasses.IsValidIndex(NextPhaseIndex) || !PhaseHPRatio.IsValidIndex(CurrentPhaseIndex)) return;

	// HP 비율이 현재 페이즈의 전환 기준 이하로 내려가면 다음 페이즈로 전환
	const float HPRatio = CurrentHP / MaxHP;
	if (HPRatio <= PhaseHPRatio[CurrentPhaseIndex]){
		ChangePhase(NextPhaseIndex);
	}
}

void ATGBossBase::ApplyBossDamage(float DamageAmount)
{
	if (DamageAmount <= 0.f || CurrentHP <= 0.f) return;

	CurrentHP = FMath::Clamp(CurrentHP - DamageAmount, 0.0f, MaxHP);

	UE_LOG(LogTemp, Warning, TEXT("[BossBase]BossDamaged - Damage: %.1f / HP: %.1f / %.1f "),
		DamageAmount, CurrentHP, MaxHP);

	if (CurrentHP <= 0){
		Destroy();
	}
}


