// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/TGCoreBase.h"

#include "Enemies/TGNavigationManager.h"

// Sets default values
ATGCoreBase::ATGCoreBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(SceneRoot);
}

// Called when the game starts or when spawned
void ATGCoreBase::BeginPlay()
{
	Super::BeginPlay();

	if (ATGNavigationManager* NavigationManager = ATGNavigationManager::Get(this)){
		NavigationManager->SetCoreActor(this);
	}
}

void ATGCoreBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ATGNavigationManager* NavigationManager = ATGNavigationManager::Get(this)){
		NavigationManager->ClearCoreActor(this);
	}

	Super::EndPlay(EndPlayReason);
}


