// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Pattern/TGPlayerSequenceSpherePattern.h"

#include "Enemies/TGBossBase.h"
#include "Enemies/TGMissile.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/TGPlayer.h"

UTGPlayerSequenceSpherePattern::UTGPlayerSequenceSpherePattern() :
	WarningSpawnDuration(1.5f),
	WarningSpawnInterval(0.3f),
	AttackDelayAfterWarning(0.f)
{
	static ConstructorHelpers::FObjectFinder<USoundBase> AttackSoundAsset(
		TEXT("/Game/Enemies/Sound/Enemy_Shot_00.Enemy_Shot_00"));
	if (AttackSoundAsset.Succeeded()){
		AttackSound = AttackSoundAsset.Object;
	}
}

void UTGPlayerSequenceSpherePattern::StartPattern(float WarningDrawTime)
{
	StopPattern();

	if (!OwnerBoss) return;

	UWorld* World = OwnerBoss->GetWorld();
	if (!World) return;

	AttackDelayAfterWarning = WarningDrawTime;

	// WarningSpawnInterval마다 플레이어 위치에 경고 생성
	World->GetTimerManager().SetTimer(
		WarningSpawnTimerHandle,
		this,
		&UTGPlayerSequenceSpherePattern::SpawnWarningAtPlayerLocation,
		WarningSpawnInterval,
		true,
		0.f
	);

	// 공격 생성 종료 시점
	World->GetTimerManager().SetTimer(
		WarningSpawnStopTimerHandle,
		this,
		&UTGPlayerSequenceSpherePattern::StopWarningSpawn,
		WarningSpawnDuration,
		false
	);
}

void UTGPlayerSequenceSpherePattern::StopPattern()
{
	if (!OwnerBoss) return;

	UWorld* World = OwnerBoss->GetWorld();
	if (!World) return;

	// Timer 정리
	World->GetTimerManager().ClearTimer(WarningSpawnTimerHandle);
	World->GetTimerManager().ClearTimer(WarningSpawnStopTimerHandle);

	for (const TPair<TObjectPtr<ATGMissile>, TObjectPtr<AActor>>& Pair : MissileWarningActors){
		if (Pair.Key){
			Pair.Key->OnMissileHit.RemoveDynamic(this, &UTGPlayerSequenceSpherePattern::HandleMissileHit);
			Pair.Key->OnMissileExpired.RemoveDynamic(this, &UTGPlayerSequenceSpherePattern::HandleMissileExpired);
		}

		if (Pair.Value){
			Pair.Value->Destroy();
		}
	}

	MissileWarningActors.Empty();
}

void UTGPlayerSequenceSpherePattern::CollectDamageTargets(TArray<AActor*>& OutTargets) const
{
	UWorld* World = nullptr;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TArray<AActor*> IgnoreActors;

	if (!PrepareOverlapQuery(World, ObjectTypes, IgnoreActors)) return;

	// 구형 범위 탐색
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

void UTGPlayerSequenceSpherePattern::SpawnWarningAtPlayerLocation()
{
	if (!OwnerBoss) return;

	UWorld* World = OwnerBoss->GetWorld();
	if (!World) return;

	ATGPlayer* Player = OwnerBoss->GetPlayer();
	if (!Player) return;

	// Player 위치 얻어옴
	const FVector NewAttackLocation = Player->GetActorLocation();

	// 구형 경고 범위
	AActor* WarningActor = SpawnSphereWarning(NewAttackLocation, WarningRadius);
	LaunchMissileAtLocation(NewAttackLocation, WarningActor);
}

void UTGPlayerSequenceSpherePattern::StopWarningSpawn()
{
	if (!OwnerBoss) return;

	UWorld* World = OwnerBoss->GetWorld();
	if (!World) return;

	World->GetTimerManager().ClearTimer(WarningSpawnTimerHandle);
}

void UTGPlayerSequenceSpherePattern::HandleMissileHit(ATGMissile* Missile, const FHitResult& HitResult)
{
	if (!Missile) return;

	Missile->OnMissileExpired.RemoveDynamic(this, &UTGPlayerSequenceSpherePattern::HandleMissileExpired);

	if (TObjectPtr<AActor>* WarningActor = MissileWarningActors.Find(Missile)){
		if (*WarningActor)
			(*WarningActor)->Destroy();
	}

	MissileWarningActors.Remove(Missile);

	AttackLocation = HitResult.ImpactPoint;
	ExecuteAttack();
}

void UTGPlayerSequenceSpherePattern::HandleMissileExpired(ATGMissile* Missile)
{
	if (!Missile) return;

	Missile->OnMissileHit.RemoveDynamic(this, &UTGPlayerSequenceSpherePattern::HandleMissileHit);

	if (TObjectPtr<AActor>* WarningActor = MissileWarningActors.Find(Missile)){
		if (*WarningActor)
			(*WarningActor)->Destroy();
	}

	MissileWarningActors.Remove(Missile);
}

void UTGPlayerSequenceSpherePattern::LaunchMissileAtLocation(FVector InAttackLocaion, AActor* WarningActor)
{
	if (!OwnerBoss || !OwnerBoss->GetMissileClass()) return;

	UWorld* World = OwnerBoss->GetWorld();
	if (!World) return;

	FTransform LaunchTransform = OwnerBoss->GetActorTransform();
	LaunchTransform.AddToTranslation(FVector::UpVector * 600.f);

	const FVector Direction = (InAttackLocaion - LaunchTransform.GetLocation()).GetSafeNormal();
	LaunchTransform.SetRotation(Direction.Rotation().Quaternion());

	ATGMissile* Missile = World->SpawnActor<ATGMissile>(OwnerBoss->GetMissileClass(), LaunchTransform);
	if (!Missile) return;

	MissileWarningActors.Add(Missile, WarningActor);

	Missile->OnMissileHit.AddDynamic(this, &UTGPlayerSequenceSpherePattern::HandleMissileHit);
	Missile->OnMissileExpired.AddDynamic(this, &UTGPlayerSequenceSpherePattern::HandleMissileExpired);

	FTGMissileParams MissileParams;
	Missile->SetHomingLocation(InAttackLocaion, 2.f, 50.f);
	Missile->Launch(MissileParams);
}

void UTGPlayerSequenceSpherePattern::ExecuteAttackAtLocation(FVector InAttackLocation)
{
	AttackLocation = InAttackLocation;
	ExecuteAttack();
}
