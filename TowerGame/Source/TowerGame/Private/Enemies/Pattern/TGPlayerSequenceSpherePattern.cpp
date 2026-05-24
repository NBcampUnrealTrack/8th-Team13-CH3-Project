// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Pattern/TGPlayerSequenceSpherePattern.h"

#include "Enemies/TGBossBase.h"
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
	Super::StopPattern();

	if (!OwnerBoss) return;

	UWorld* World = OwnerBoss->GetWorld();
	if (!World) return;

	// Timer 정리
	World->GetTimerManager().ClearTimer(WarningSpawnTimerHandle);
	World->GetTimerManager().ClearTimer(WarningSpawnStopTimerHandle);
	for (FTimerHandle& TimerHandle : SequenceAttackTimerHandles){
		World->GetTimerManager().ClearTimer(TimerHandle);
	}

	// 사용 변수 초기화
	SequenceAttackTimerHandles.Empty();
	WarningLocations.Empty();
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
	WarningLocations.Add(NewAttackLocation);

	// 구형 경고 범위
	SpawnSphereWarning(NewAttackLocation, WarningRadius, AttackDelayAfterWarning);

	// Timer & 델리게이트로 공격 실행
	FTimerHandle AttackTimerHandle;
	FTimerDelegate AttackTimerDelegate;
	AttackTimerDelegate.BindUObject(
		this,
		&UTGPlayerSequenceSpherePattern::ExecuteAttackAtLocation,
		NewAttackLocation
	);

	World->GetTimerManager().SetTimer(
		AttackTimerHandle,
		AttackTimerDelegate,
		AttackDelayAfterWarning,
		false
	);

	SequenceAttackTimerHandles.Add(AttackTimerHandle);

}

void UTGPlayerSequenceSpherePattern::StopWarningSpawn()
{
	if (!OwnerBoss) return;

	UWorld* World = OwnerBoss->GetWorld();
	if (!World) return;

	World->GetTimerManager().ClearTimer(WarningSpawnTimerHandle);
}

void UTGPlayerSequenceSpherePattern::ExecuteAttackAtLocation(FVector InAttackLocation)
{
	AttackLocation = InAttackLocation;
	ExecuteAttack();
}
