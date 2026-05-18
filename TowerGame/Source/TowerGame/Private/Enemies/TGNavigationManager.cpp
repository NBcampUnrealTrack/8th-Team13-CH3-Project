// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/TGNavigationManager.h"

#include "EngineUtils.h"
#include "NavigationSystem.h"
#include "Enemies/TGCoreBase.h"
#include "Enemies/TGEnemyBase.h"

TWeakObjectPtr<ATGNavigationManager> ATGNavigationManager::Instance;

// Sets default values
ATGNavigationManager::ATGNavigationManager() : CurrentCoreActor(nullptr)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ATGNavigationManager::BeginPlay()
{
	Super::BeginPlay();

	if (Instance.IsValid() && Instance.Get() != this && Instance->GetWorld() == GetWorld()){
		UE_LOG(LogTemp, Error, TEXT("NavigationManager가 World에 2개 이상 존재합니다"));
	}

	Instance = this;

	// NavMesh 갱신 완료 델리게이트
	if (UNavigationSystemV1* NavigationSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld())){
		NavigationSystem->OnNavigationGenerationFinishedDelegate.AddDynamic(
			this, &ATGNavigationManager::HandleNavigationGenerationFinished);
	}
}

void ATGNavigationManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Instance.Get() == this){
		Instance.Reset();
	}

	Super::EndPlay(EndPlayReason);
}

ATGNavigationManager* ATGNavigationManager::Get(const UObject* WorldContextObject)
{
	if (!WorldContextObject) return nullptr;

	UWorld* World = WorldContextObject->GetWorld();
	if (!World) return nullptr;

	// 등록된 인스턴스가 있고 같은 World 소속이면 그대로 사용한다
	if (Instance.IsValid() && Instance->GetWorld() == World->GetWorld()){
		return Instance.Get();
	}

	// 아직 등록된 인스턴스가 없거나 World 가 다르면 현재 World에서 탐색한다
	for (TActorIterator<ATGNavigationManager> It(World); It; ++It){
		Instance = *It;
		return Instance.Get();
	}

	return nullptr;
}

void ATGNavigationManager::AddCoreActor(ATGCoreBase* InCoreActor)
{
	if (!InCoreActor) return;

	// CoreBase을 추가하고 현재 CoreBase가 없다면 새로 지정
	CoreActors.AddUnique(InCoreActor);
	if (!CurrentCoreActor){
		CurrentCoreActor = CoreActors[0];
		OnCurrentCoreChanged.Broadcast(CurrentCoreActor);
		RepathAllEnemies();
	}
}

void ATGNavigationManager::RemoveCoreActor(ATGCoreBase* InCoreActor)
{
	if (!InCoreActor) return;

	const bool bWasCurrentCore = (CurrentCoreActor == InCoreActor);

	// CoreBase을 지정하고 CurrentCoreBase가 없어졌다면 새로 지정
	CoreActors.Remove(InCoreActor);
	if (bWasCurrentCore){
		CurrentCoreActor = CoreActors.Num() > 0 ? CoreActors[0] : nullptr;
		OnCurrentCoreChanged.Broadcast(CurrentCoreActor);
		RepathAllEnemies();
	}
}

void ATGNavigationManager::RegisterEnemy(ATGEnemyBase* Enemy)
{
	if (!Enemy) return;

	AliveEnemies.AddUnique(Enemy);
}

void ATGNavigationManager::UnRegisterEnemy(ATGEnemyBase* Enemy)
{
	if (!Enemy) return;

	AliveEnemies.Remove(Enemy);
}

FVector ATGNavigationManager::GetCoreLocation() const
{
	if (!CurrentCoreActor){
		return FVector::ZeroVector;
	}

	return CurrentCoreActor->GetActorLocation();
}

ATGCoreBase* ATGNavigationManager::GetCurrentCoreActor() const
{
	return CurrentCoreActor;
}

void ATGNavigationManager::NotifyBuildingPlaced()
{
	bPendingRepathAfterNavGeneration = true;
}

void ATGNavigationManager::RepathAllEnemies()
{
	for (ATGEnemyBase* Enemy: AliveEnemies)
	{
		if (!Enemy) continue;

		Enemy->RequestRepath();
	}
}

// NavMesh 갱신 이후 경로 탐색
void ATGNavigationManager::HandleNavigationGenerationFinished(ANavigationData* NavData)
{
	if (!bPendingRepathAfterNavGeneration) return;
	bPendingRepathAfterNavGeneration = false;

	RepathAllEnemies();
}
