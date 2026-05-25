// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Pattern/TGAreaKnockbackPattern.h"

#include "Enemies/TGBossBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/TGPlayer.h"

UTGAreaKnockbackPattern::UTGAreaKnockbackPattern() :
ActiveDuration(1.f),
HitCheckInterval(0.1f),
KnockbackStrength(1600.f),
KnockbackUpStrength(200.f)
{
}

void UTGAreaKnockbackPattern::StartPattern(float WarningDrawTime)
{
	StopPattern();

	// 경고 범위 표시
	GetAttackLocation();
	SpawnSphereWarning(AttackLocation, WarningRadius, WarningDrawTime);

	if (!OwnerBoss) return;

	UWorld* World = OwnerBoss->GetWorld();
	if (!World) return;

	// 공격 타이머
	World->GetTimerManager().SetTimer(
		AttackDelayTimerHandle,
		this,
		&UTGAreaKnockbackPattern::StartActiveAttack,
		WarningDrawTime,
		false
	);
}

void UTGAreaKnockbackPattern::StopPattern()
{
	if (!OwnerBoss) return;

	OwnerBoss->StopPatternYawRotation();

	UWorld* World = OwnerBoss->GetWorld();
	if (!World) return;

	// Timer 정리
	World->GetTimerManager().ClearTimer(AttackDelayTimerHandle);
	World->GetTimerManager().ClearTimer(ActiveAttackTimerHandle);
	World->GetTimerManager().ClearTimer(ActiveAttackEndTimerHandle);

	OnPatternFinished.Broadcast();
}

void UTGAreaKnockbackPattern::ExecuteAttack()
{
	GetAttackLocation();

	// 공격 범위 내 대상 탐색
	TArray<AActor*> DamageTargets;
	CollectDamageTargets(DamageTargets);

	// Sound 탐색 후 주석 해제
	//PlayAttackFeedback(AttackLocation);
	// 데미지 & 넉백
	ApplyDamageToTargets(DamageTargets, AttackDamage);
	ApplyKnockbackToTargets(DamageTargets);
}

void UTGAreaKnockbackPattern::GetAttackLocation()
{
	Super::GetAttackLocation();

	if (!OwnerBoss) return;
	AttackLocation = OwnerBoss->GetActorLocation();
}

void UTGAreaKnockbackPattern::CollectDamageTargets(TArray<AActor*>& OutTargets) const
{
	UWorld* World = nullptr;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TArray<AActor*> IgnoreActors;

	if (!PrepareOverlapQuery(World, ObjectTypes, IgnoreActors)) return;

	// 공격 대상 탐색 (구형)
	UKismetSystemLibrary::SphereOverlapActors(
		World,
		AttackLocation,
		WarningRadius,
		ObjectTypes,
		nullptr,
		IgnoreActors,
		OutTargets
	);
}

void UTGAreaKnockbackPattern::StartActiveAttack()
{
	if (!OwnerBoss) return;

	UWorld* World = OwnerBoss->GetWorld();
	if (!World) return;

	// 한바퀴 회전
	const float FullTurnRotationSpeed = ActiveDuration > 0.f ? 360.f / ActiveDuration : 0.f;
	OwnerBoss->StartPatternYawRotation(FullTurnRotationSpeed);

	// 범위 내 공격 시작
	World->GetTimerManager().SetTimer(
		ActiveAttackTimerHandle,
		this,
		&UTGAreaKnockbackPattern::ExecuteAttack,
		HitCheckInterval,
		true,
		0
	);

	// 범위 공격 종료 타이머
	World->GetTimerManager().SetTimer(
		ActiveAttackEndTimerHandle,
		this,
		&UTGAreaKnockbackPattern::StopPattern,
		ActiveDuration,
		false
	);
}

void UTGAreaKnockbackPattern::ApplyKnockbackToTargets(const TArray<AActor*>& Targets) const
{
	if (!OwnerBoss) return;

	for (AActor* Target : Targets){
		ATGPlayer* Player = Cast<ATGPlayer>(Target);
		if (!Player) continue;

		// Player 날릴 방향
		FVector KnockbackDirection = Player->GetActorLocation() - OwnerBoss->GetActorLocation();
		KnockbackDirection.Z = 0.f;

		if (!KnockbackDirection.Normalize()){
			KnockbackDirection = OwnerBoss->GetActorForwardVector();
		}

		const FVector LaunchVelocity =
			KnockbackDirection * KnockbackStrength + FVector::UpVector * KnockbackUpStrength;

		// Player 밀치기
		Player->LaunchCharacter(LaunchVelocity, true, true);
	}
}
