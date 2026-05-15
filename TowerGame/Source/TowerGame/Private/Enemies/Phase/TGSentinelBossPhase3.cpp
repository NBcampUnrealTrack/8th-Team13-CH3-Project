// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Phase/TGSentinelBossPhase3.h"

#include "Enemies/TGBossBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/TGPlayer.h"

UTGSentinelBossPhase3::UTGSentinelBossPhase3() :
	WarningRadius(250.f),
	WarningDrawTime(1.5f),
	AttackDelay(1.5f),
	AttackDamage(20.f),
	PendingAttackLocation(FVector::ZeroVector)
{
}

void UTGSentinelBossPhase3::EnterPhase()
{
	Super::EnterPhase();
	UE_LOG(LogTemp, Warning, TEXT("Sentinel Boss Phase 3 Enter"));
}

void UTGSentinelBossPhase3::ExitPhase()
{
	UE_LOG(LogTemp, Warning, TEXT("Sentinel Boss Phase 3 Exit"));
	Super::ExitPhase();
}

void UTGSentinelBossPhase3::ExecutePattern()
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
		&UTGSentinelBossPhase3::ExecuteDelayedAttack,
		AttackDelay,
		false
	);

	UE_LOG(LogTemp, Warning, TEXT("Sentinel Boss Phase 3 Pattern"));
}

void UTGSentinelBossPhase3::ExecuteDelayedAttack()
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
}
