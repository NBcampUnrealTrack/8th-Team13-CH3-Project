// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Pattern/TGSphereAreaPattern.h"

#include "Enemies/TGBossBase.h"
#include "Enemies/TGMissile.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/TGPlayer.h"

UTGSphereAreaPattern::UTGSphereAreaPattern()
{
	// 공격 사운드/이펙트 기본값은 UTGPatternBase 생성자에서 지정
}

void UTGSphereAreaPattern::StartPattern(float WarningDrawTime)
{
	StopPattern();

	// 공격 범위 경고 표시
	GetAttackLocation();
	ActiveWarningActor = SpawnSphereWarning(AttackLocation, WarningRadius);

	if (!OwnerBoss) return;

	UWorld* World = OwnerBoss->GetWorld();
	if (!World) return;

	// 지연 시간 후 공격 판정
	World->GetTimerManager().SetTimer(
		MissileLaunchTimerHandle,
		this,
		&UTGSphereAreaPattern::LaunchMissileToWarningLocation,
		WarningDrawTime,
		false
	);
}

void UTGSphereAreaPattern::StopPattern()
{
	if (!OwnerBoss) return;

	if (UWorld* World = OwnerBoss->GetWorld()){
		World->GetTimerManager().ClearTimer(MissileLaunchTimerHandle);
	}

	// Delegate 제거
	if (ActiveMissile){
		ActiveMissile->OnMissileHit.RemoveDynamic(this, &UTGSphereAreaPattern::HandleMissileHit);
		ActiveMissile->OnMissileExpired.RemoveDynamic(this, &UTGSphereAreaPattern::HandleMissileExpired);
		ActiveMissile = nullptr;
	}

	// 경고 범위 Actor 제거
	if (ActiveWarningActor){
		ActiveWarningActor->Destroy();
		ActiveWarningActor = nullptr;
	}
}

void UTGSphereAreaPattern::GetAttackLocation()
{
	AttackLocation = FVector::ZeroVector;

	if (!OwnerBoss) return;

	ATGPlayer* Player = OwnerBoss->GetPlayer();
	if (!Player) return;

	AttackLocation = Player->GetActorLocation();
}

void UTGSphereAreaPattern::CollectDamageTargets(TArray<AActor*>& OutTargets) const
{
	UWorld* World = nullptr;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TArray<AActor*> IgnoreActors;

	if (!PrepareOverlapQuery(World, ObjectTypes, IgnoreActors)) return;

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

void UTGSphereAreaPattern::HandleMissileHit(ATGMissile* Missile, const FHitResult& HitResult)
{
	// 제거 델리게이트 해제
	if (Missile){
		Missile->OnMissileExpired.RemoveDynamic(this, &UTGSphereAreaPattern::HandleMissileExpired);
	}

	// 경고 범위 제거
	if (ActiveWarningActor){
		ActiveWarningActor->Destroy();
		ActiveWarningActor = nullptr;
	}

	if (Missile == ActiveMissile){
		ActiveMissile = nullptr;
	}

	// 충돌 위치를 공격 위치로 지정
	AttackLocation = HitResult.ImpactPoint;
	ExecuteAttack();
	OnPatternFinished.Broadcast();
}

void UTGSphereAreaPattern::HandleMissileExpired(ATGMissile* Missile)
{
	// 충돌 델리게이트 제거
	if (Missile){
		Missile->OnMissileHit.RemoveDynamic(this, &UTGSphereAreaPattern::HandleMissileHit);
	}

	if (Missile == ActiveMissile){
		ActiveMissile = nullptr;
	}

	// 경고 범위 제거
	if (ActiveWarningActor){
		ActiveWarningActor->Destroy();
		ActiveWarningActor = nullptr;
	}

	OnPatternFinished.Broadcast();
}

void UTGSphereAreaPattern::LaunchMissileToWarningLocation()
{
	if (!OwnerBoss || !OwnerBoss->GetMissileClass()) return;

	UWorld* World = OwnerBoss->GetWorld();
	if (!World) return;

	// 미사일 생성 위치
	FTransform LaunchTransform = OwnerBoss->GetActorTransform();
	LaunchTransform.AddToTranslation(FVector::UpVector * 600.f);

	// 이동 방향
	const FVector Direction = (AttackLocation - LaunchTransform.GetLocation()).GetSafeNormal();
	LaunchTransform.SetRotation(Direction.Rotation().Quaternion());

	// 미사일 생성
	ActiveMissile = World->SpawnActor<ATGMissile>(OwnerBoss->GetMissileClass(), LaunchTransform);
	if (!ActiveMissile) return;

	// 델리게이트 등록
	ActiveMissile->OnMissileHit.AddDynamic(this, &UTGSphereAreaPattern::HandleMissileHit);
	ActiveMissile->OnMissileExpired.AddDynamic(this, &UTGSphereAreaPattern::HandleMissileExpired);

	FTGMissileParams MissileParams;
	ActiveMissile->SetHomingLocation(AttackLocation, 2.f, 50.f);
	ActiveMissile->Launch(MissileParams);
}
