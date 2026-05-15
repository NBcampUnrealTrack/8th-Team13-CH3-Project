// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/TGBossBase.h"

// Sets default values
ATGBossBase::ATGBossBase() : MaxHP(1000), CurrentHP(0), CurrentPhase(nullptr), CurrentPhaseIndex(INDEX_NONE)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ATGBossBase::BeginPlay()
{
	Super::BeginPlay();

	CurrentHP = MaxHP;
}

void ATGBossBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CurrentPhase = nullptr;
	Phases.Reset();

	Super::EndPlay(EndPlayReason);
}

float ATGBossBase::GetCurrentHP() const
{
	return CurrentHP;
}

float ATGBossBase::GetMaxHP() const
{
	return MaxHP;
}


