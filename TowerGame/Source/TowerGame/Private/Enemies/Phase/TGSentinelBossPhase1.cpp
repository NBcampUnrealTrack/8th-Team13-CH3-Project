// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Phase/TGSentinelBossPhase1.h"

#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Enemies/TGBossBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/TGPlayer.h"

UTGSentinelBossPhase1::UTGSentinelBossPhase1() :
	WarningRadius(300.f),
	WarningDrawTime(1.5f),
	AttackDelay(1.5),
	AttackDamage(10),
	PendingAttackLocation(FVector::ZeroVector)
{
}

void UTGSentinelBossPhase1::EnterPhase()
{
	Super::EnterPhase();
	UE_LOG(LogTemp, Warning, TEXT("Sentinel Boss Phase 1 Enter"));
}

void UTGSentinelBossPhase1::ExitPhase()
{
	UE_LOG(LogTemp, Warning, TEXT("Sentinel Boss Phase 1 Exit"));
	Super::ExitPhase();
}

void UTGSentinelBossPhase1::ExecutePattern()
{
	if (!OwnerBoss) return;

	ATGPlayer* Player = OwnerBoss->GetPlayer();
	if (!Player) return;

	UWorld* World = OwnerBoss->GetWorld();
	if (!World) return;

	// Character 기준 바닥 근처로 보정해서 경고 범위를 표시한다.
	const float CapsuleHalfHeight = Player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	PendingAttackLocation = Player->GetActorLocation();
	PendingAttackLocation.Z -= CapsuleHalfHeight;
	DrawDebugCylinder(
		World,
		PendingAttackLocation,
		PendingAttackLocation + FVector(0.f, 0.f, 40.f),
		WarningRadius,
		48,
		FColor::Red,
		false,
		WarningDrawTime,
		0,
		4.f
	);

	FTimerHandle AttackDelayTimerHandle;
	World->GetTimerManager().SetTimer(
		AttackDelayTimerHandle,
		this,
		&UTGSentinelBossPhase1::ExecuteDelayedAttack,
		AttackDelay,
		false
	);
}

void UTGSentinelBossPhase1::ExecuteDelayedAttack()
{
	ATGPlayer* Player = OwnerBoss->GetPlayer();
	if (!Player) return;

	// 원형 범위 - Dist2D
	const float Distance = FVector::Dist2D(PendingAttackLocation, Player->GetActorLocation());
	if (Distance > WarningRadius) return;

	// 범위 내 Player에게 피해를 적용 (Core 도 적용 받도록 추가 예정)
	UGameplayStatics::ApplyDamage(
		Player,
		AttackDamage,
		nullptr,
		OwnerBoss,
		nullptr
	);
}
