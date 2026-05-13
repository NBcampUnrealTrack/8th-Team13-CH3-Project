// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/TGAttackEnemyBase.h"

#include "AIController.h"
#include "TGMountedTower.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Player/TGPlayer.h"

ATGAttackEnemyBase::ATGAttackEnemyBase() :
	PlayerDetectRange(400),
	PlayerDetectInterval(0.2f),
	AreaAttackRange(550),
	AreaAttackDelay(0.8f),
	AreaAttackDamage(1),
	KnockbackForce(800),
	DebuffDuration(2)
{
}

void ATGAttackEnemyBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld()){
		World->GetTimerManager().ClearTimer(PlayerDetectTimerHandle);
		World->GetTimerManager().ClearTimer(AreaAttackTimerHandle);
	}

	Super::EndPlay(EndPlayReason);
}

void ATGAttackEnemyBase::RequestRepath()
{
	if (UWorld* World = GetWorld()){
		// Core / Tower로 이동하는 동안 Player 탐지 시작
		World->GetTimerManager().ClearTimer(PlayerDetectTimerHandle);
		World->GetTimerManager().SetTimer(
			PlayerDetectTimerHandle,
			this,
			&ATGAttackEnemyBase::DetectPlayerInRange,
			PlayerDetectInterval,
			true
		);
	}

	Super::RequestRepath();
}

void ATGAttackEnemyBase::StartAttack()
{
	if (UWorld* World = GetWorld()){
		// Core / Tower 공격 중에는 Player 탐지 중단
		World->GetTimerManager().ClearTimer(PlayerDetectTimerHandle);
	}

	Super::StartAttack();
}

void ATGAttackEnemyBase::DetectPlayerInRange()
{
	// Player 공격 대기 중에는 중복 탐지 방지
	UWorld* World = GetWorld();
	if (!World || World->GetTimerManager().IsTimerActive(AreaAttackTimerHandle)) return;

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!PlayerPawn) return;

	const float Distance = FVector::Dist2D(GetActorLocation(), PlayerPawn->GetActorLocation());
	if (Distance > PlayerDetectRange) return;

	StartPlayerAttack(PlayerPawn);
}

void ATGAttackEnemyBase::StartPlayerAttack(AActor* Player)
{
	if (!Player) return;

	UWorld* World = GetWorld();
	if (!World) return;

	// Player를 발견하면 탐지를 멈춤
	World->GetTimerManager().ClearTimer(PlayerDetectTimerHandle);

	StopAttack();

	if (AAIController* AIController = Cast<AAIController>(GetController())){
		// Player 공격 과정에서는 이동 중단
		AIController->ReceiveMoveCompleted.RemoveAll(this);
		AIController->StopMovement();
	}

	World->GetTimerManager().ClearTimer(AreaAttackTimerHandle);
	World->GetTimerManager().SetTimer(
		AreaAttackTimerHandle,
		this,
		&ATGAttackEnemyBase::AreaAttack,
		AreaAttackDelay,
		false
	);
}

void ATGAttackEnemyBase::StopPlayerAttack()
{
	if (UWorld* World = GetWorld()){
		World->GetTimerManager().ClearTimer(AreaAttackTimerHandle);
	}

	// Player 공격 이후 이동 재개
	RequestRepath();
}

void ATGAttackEnemyBase::AreaAttack()
{
	UWorld* World = GetWorld();
	if (!World){
		StopPlayerAttack();
		return;
	}

	TArray<FOverlapResult> OverlapResults;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	// Enemy 중심 원형 범위 공격
	const FCollisionShape AttackShape = FCollisionShape::MakeSphere(AreaAttackRange);
	const FCollisionObjectQueryParams ObjectQueryParams(ECC_Pawn);

	const bool bHit = World->OverlapMultiByObjectType(
		OverlapResults,
		GetActorLocation(),
		FQuat::Identity,
		ObjectQueryParams,
		AttackShape,
		QueryParams
	);

	if (bHit){
		TSet<AActor*> HitActors;
		for (const FOverlapResult& Result : OverlapResults){
			AActor* Target = Result.GetActor();
			if (!Target || HitActors.Contains(Target) || !CanAreaAttackTarget(Target)) continue;

			// 동일 Actor 중복 피격 방지
			HitActors.Add(Target);
			ApplyAreaAttackEffect(Target);
		}
	}

	StopPlayerAttack();
}

bool ATGAttackEnemyBase::CanAreaAttackTarget(AActor* Target) const
{
	return Cast<ATGPlayer>(Target) != nullptr;
}

void ATGAttackEnemyBase::ApplyAreaAttackEffect(AActor* Target)
{
	if (!Target) return;

	ATGPlayer* Player = Cast<ATGPlayer>(Target);
	if (Player){
		ApplyPlayerAttackEffect(Player);
		return;
	}

	ATGMountedTower* Tower = Cast<ATGMountedTower>(Target);
	if (Tower){
		ApplyTowerAttackEffect(Tower);
		return;
	}
}

void ATGAttackEnemyBase::ApplyPlayerAttackEffect(ATGPlayer* Player)
{
	if (!Player) return;

	// 데미지
	UGameplayStatics::ApplyDamage(
		Player,
		AreaAttackDamage,
		GetController(),
		this,
		nullptr
	);

	if (KnockbackForce <= 0) return;

	// Knockback
	FVector KnockbackDirection = Player->GetActorLocation() - GetActorLocation();
	KnockbackDirection.Z = 0.15f;
	KnockbackDirection.Normalize();

	Player->LaunchCharacter(
		KnockbackDirection * KnockbackForce, true, true);

	// TODO Player Debuff API 연결
}

void ATGAttackEnemyBase::ApplyTowerAttackEffect(ATGMountedTower* Tower)
{
	if (!Tower) return;

	// TODO Tower 무력화 API 연결
}
