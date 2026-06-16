// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemies/TGSwarmSpawner.h"
#include "Enemies/TGSwarmEnemy.h"
#include "Components/SceneComponent.h"

ATGSwarmSpawner::ATGSwarmSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
}

void ATGSwarmSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoStart)
	{
		StartSpawning();
	}
}

void ATGSwarmSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopSpawning();
	Super::EndPlay(EndPlayReason);
}

void ATGSwarmSpawner::StartSpawning()
{
	if (!EnemyClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TGSwarmSpawner] EnemyClass가 지정되지 않아 스폰을 시작할 수 없습니다."));
		return;
	}

	SpawnedCount = 0;
	GetWorldTimerManager().SetTimer(
		SpawnTimerHandle,
		this,
		&ATGSwarmSpawner::SpawnTick,
		SpawnInterval,
		true,
		0.f
	);
}

void ATGSwarmSpawner::StopSpawning()
{
	GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
}

void ATGSwarmSpawner::SpawnTick()
{
	for (int32 i = 0; i < SpawnPerTick; ++i)
	{
		// 총량 제한 (SpawnCount <= 0 이면 무한)
		if (SpawnCount > 0 && SpawnedCount >= SpawnCount)
		{
			StopSpawning();
			return;
		}
		SpawnOne();
		SpawnedCount++;
	}
}

void ATGSwarmSpawner::SpawnOne()
{
	UWorld* World = GetWorld();
	if (!World || !EnemyClass) return;

	const FVector Origin = GetActorLocation();
	// 반경 내 균등 분포(원형) 랜덤 위치
	const float Angle = FMath::FRandRange(0.f, 2.f * PI);
	const float Radius = SpawnRadius * FMath::Sqrt(FMath::FRand());
	const FVector SpawnLocation = Origin + FVector(FMath::Cos(Angle) * Radius, FMath::Sin(Angle) * Radius, 0.f);

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	World->SpawnActor<ATGSwarmEnemy>(EnemyClass, SpawnLocation, FRotator::ZeroRotator, Params);
}
