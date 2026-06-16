// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemies/TGCombatComponent.h"
#include "Core/GameFlow/TGGameMode.h"
#include "Kismet/GameplayStatics.h"

UTGCombatComponent::UTGCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTGCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHP = MaxHP;
	bDead = false;
}

float UTGCombatComponent::ApplyDamage(float Amount)
{
	if (bDead || Amount <= 0.f) return 0.f;

	const float Applied = FMath::Min(Amount, CurrentHP);
	CurrentHP -= Applied;
	OnHpChanged.Broadcast(CurrentHP, MaxHP);

	if (CurrentHP <= 0.f)
	{
		bDead = true;

		// 처치 시 경험치 지급 (Step 2의 진행도 시스템 재사용)
		if (ATGGameMode* GM = Cast<ATGGameMode>(UGameplayStatics::GetGameMode(this)))
		{
			GM->AddExp(ExpDropAmount);
		}

		OnDied.Broadcast(GetOwner());
	}

	return Applied;
}
