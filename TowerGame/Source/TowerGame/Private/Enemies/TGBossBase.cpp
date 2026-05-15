// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/TGBossBase.h"

#include "Enemies/TGBossPhaseBase.h"

// Sets default values
ATGBossBase::ATGBossBase() : MaxHP(1), CurrentHP(0), CurrentPhase(nullptr), CurrentPhaseIndex(INDEX_NONE)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ATGBossBase::BeginPlay()
{
	Super::BeginPlay();

	CurrentHP = MaxHP;
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

	// UObject 기반 페이즈 생성
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
	if (!PhaseClasses.IsValidIndex(NextPhaseIndex) || !PhaseClasses.IsValidIndex(CurrentPhaseIndex)) return;

	// HP 비율이 현재 페이즈의 전환 기준 이하로 내려가면 다음 페이즈로 전환
	const float HPRatio = CurrentHP / MaxHP;
	if (HPRatio <= PhaseHPRatio[CurrentPhaseIndex]){
		ChangePhase(NextPhaseIndex);
	}
}

void ATGBossBase::ApplyBossDamage(float DamageAmount)
{
	if (DamageAmount <= 0 || CurrentHP <= 0) return;

	CurrentHP = FMath::Clamp(CurrentHP - DamageAmount, 0.0f, MaxHP);

	UE_LOG(LogTemp, Warning, TEXT("[BossBase]BossDamaged - Damage: %.1f / HP: %.1f / %.1f "),
		DamageAmount, CurrentHP, MaxHP);

	if (CurrentHP <= 0){
		Destroy();
	}
}


