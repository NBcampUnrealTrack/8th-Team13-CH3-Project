// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/TGEnemySpawner.h"

#include "Components/CapsuleComponent.h"
#include "Enemies/TGEnemyBase.h"
#include "Enemies/TGNavigationManager.h"
#include "Enemies/TGWaveManager.h"

// Sets default values
ATGEnemySpawner::ATGEnemySpawner() : SpawnLocationOffset(FVector::ZeroVector), LastSpawnedEnemyClass(nullptr)
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

// EnemyBase의 HalfHeight를 가져와 저장
void ATGEnemySpawner::UpdateSpawnLocationOffset(TSubclassOf<ATGEnemyBase> EnemyClass)
{
	if (!EnemyClass) return;

	const ATGEnemyBase* DefaultEnemy = EnemyClass->GetDefaultObject<ATGEnemyBase>();
	if (!DefaultEnemy) return;

	const UCapsuleComponent* CapsuleCollision = DefaultEnemy->GetCapsuleComponent();
	if (!CapsuleCollision) return;

	SpawnLocationOffset = FVector(0, 0, CapsuleCollision->GetScaledCapsuleHalfHeight());

	// Enemy의 활동 위치(지상, 공중)에 따른 보정
	SpawnLocationOffset += DefaultEnemy->GetNavigationLocationOffset();
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

	// Spawn하는 Enemy가 달라진 경우 Offset 재 계산
	if (LastSpawnedEnemyClass != EnemyClass){
		UpdateSpawnLocationOffset(EnemyClass);
		LastSpawnedEnemyClass = EnemyClass;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;

	// 생성 위치 보정
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	// 생성 위치 보정
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


