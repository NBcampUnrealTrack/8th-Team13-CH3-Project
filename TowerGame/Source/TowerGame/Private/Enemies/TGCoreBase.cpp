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

float ATGCoreBase::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	const float AppliedDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	UE_LOG(LogTemp, Warning, TEXT("CoreBase 피격 - Damage: %.1f"), AppliedDamage);

	// TODO
	// 상위 Manager에 Core 피격 이벤트 전달

	return AppliedDamage;
}

// Called when the game starts or when spawned
void ATGCoreBase::BeginPlay()
{
	Super::BeginPlay();

	if (ATGNavigationManager* NavigationManager = ATGNavigationManager::Get(this)){
		NavigationManager->AddCoreActor(this);
	}
}

void ATGCoreBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ATGNavigationManager* NavigationManager = ATGNavigationManager::Get(this)){
		NavigationManager->RemoveCoreActor(this);
	}

	Super::EndPlay(EndPlayReason);
}


