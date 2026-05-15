// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Phase/TGSentinelBossPhase2.h"

#include "Enemies/TGBossBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/TGPlayer.h"

UTGSentinelBossPhase2::UTGSentinelBossPhase2() :
	WarningRadius(150.f),
	WarningDrawTime(1.5f),
	AttackDelay(1.5f),
	AttackDamage(15.f),
	PendingAttackLocation(FVector::ZeroVector)
{
}

void UTGSentinelBossPhase2::EnterPhase()
{
	Super::EnterPhase();
	UE_LOG(LogTemp, Warning, TEXT("Sentinel Boss Phase 2 Enter"));
}

void UTGSentinelBossPhase2::ExitPhase()
{
	UE_LOG(LogTemp, Warning, TEXT("Sentinel Boss Phase 2 Exit"));
	Super::ExitPhase();
}

void UTGSentinelBossPhase2::ExecutePattern()
{
	if (!OwnerBoss) return;

	ATGPlayer* Player = OwnerBoss->GetPlayer();
	if (!Player) return;

	UWorld* World = OwnerBoss->GetWorld();
	if (!World) return;

	PendingAttackLocation = Player->GetActorLocation();

	// PlayerHalfHeight 기준 구형 범위 생성
	DrawDebugSphere(
		World,
		PendingAttackLocation,
		WarningRadius,
		32,
		FColor::Red,
		false,
		WarningDrawTime,
		0,
		1.f
	);

	FTimerHandle AttackDelayTimerHandle;
	World->GetTimerManager().SetTimer(
		AttackDelayTimerHandle,
		this,
		&UTGSentinelBossPhase2::ExecuteDelayedAttack,
		AttackDelay,
		false
	);

	UE_LOG(LogTemp, Warning, TEXT("Sentinel Boss Phase 2 Pattern"));
}

void UTGSentinelBossPhase2::ExecuteDelayedAttack()
{
	if (!OwnerBoss) return;

	ATGPlayer* Player = OwnerBoss->GetPlayer();
	if (!Player) return;

	// 구형 범위 - Dist
	const float Distance = FVector::Dist(PendingAttackLocation, Player->GetActorLocation());
	if (Distance > WarningRadius) return;

	// 범위 내 Player에게 피해를 적용 (Core 도 적용 받도록 추가 예정)
	UGameplayStatics::ApplyDamage(
		Player,
		AttackDamage,
		nullptr,
		OwnerBoss,
		nullptr
	);

	UE_LOG(LogTemp, Warning, TEXT("Sentinel Boss Phase 2 Hit Player - Damage: %.1f"), AttackDamage);
}
