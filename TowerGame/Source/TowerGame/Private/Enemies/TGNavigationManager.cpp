// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/TGNavigationManager.h"

#include "EngineUtils.h"
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

	// 등록된 인스턴스가 있고 같은 World 소속이면 그대로 사용한디
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

	CoreActors.AddUnique(InCoreActor);
	if (!CurrentCoreActor){
		CurrentCoreActor = InCoreActor;
		RepathAllEnemies();
	}
}

void ATGNavigationManager::RemoveCoreActor(ATGCoreBase* InCoreActor)
{
	if (!InCoreActor) return;

	const bool bWasCurrentCore = (CurrentCoreActor == InCoreActor);

	CoreActors.Remove(InCoreActor);
	if (bWasCurrentCore){
		CurrentCoreActor = CoreActors.Num() > 0 ? CoreActors[0] : nullptr;
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
	if (!CurrentCoreActor)
	{
		return FVector::ZeroVector;
	}

	return CurrentCoreActor->GetActorLocation();
}

void ATGNavigationManager::NotifyBuildingPlaced()
{
	RepathAllEnemies();
}

void ATGNavigationManager::RepathAllEnemies()
{
	for (ATGEnemyBase* Enemy: AliveEnemies)
	{
		if (!Enemy) continue;

		Enemy->RequestRepath();
	}
}

