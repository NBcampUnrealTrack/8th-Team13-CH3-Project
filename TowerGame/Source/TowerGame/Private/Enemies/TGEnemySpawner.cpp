// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/TGEnemySpawner.h"

#include "Enemies/TGEnemyBase.h"
#include "Enemies/TGNavigationManager.h"
#include "Enemies/TGWaveManager.h"

// Sets default values
ATGEnemySpawner::ATGEnemySpawner() : SpawnLocationOffset(0,0,88)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

}

void ATGEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	if (ATGWaveManager* WaveManager = ATGWaveManager::Get(this))
	{
		WaveManager->AddEnemySpawner(this);
	}
}

void ATGEnemySpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ATGWaveManager* WaveManager = ATGWaveManager::Get(this)){
		WaveManager->RemoveEnemySpawner(this);
	}

	Super::EndPlay(EndPlayReason);
}

ATGEnemyBase* ATGEnemySpawner::SpawnEnemy(TSubclassOf<ATGEnemyBase> EnemyClass)
{
	if (!EnemyClass) return nullptr;

	UWorld* World = GetWorld();
	if (!World) return nullptr;

	if (!NavigationManager){
		NavigationManager = ATGNavigationManager::Get(this);

		if (!NavigationManager){
			UE_LOG(LogTemp, Error, TEXT("NavigationManger을 찾지 못했습니다"));
			return nullptr;
		}
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;

	// 생성 위치 보정
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	// 생성 위치 보정 - 기본 캡슙액터의 Capsule Half Height(88)
	FTransform SpawnTransform = GetActorTransform();
	SpawnTransform.AddToTranslation(SpawnLocationOffset);

	// Enemy 생성
	ATGEnemyBase* SpawnEnemy = GetWorld()->SpawnActor<ATGEnemyBase>(
		EnemyClass, SpawnTransform, SpawnParams);

	if (!SpawnEnemy) return nullptr;

	// Observer System 등록
	SpawnEnemy->InitializeEnemy(NavigationManager);

	return SpawnEnemy;
}


