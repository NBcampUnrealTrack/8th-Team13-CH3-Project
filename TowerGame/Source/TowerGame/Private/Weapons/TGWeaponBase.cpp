// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/TGWeaponBase.h"

// Sets default values
ATGWeaponBase::ATGWeaponBase()
	: Name(TEXT("Base")), TriggerType(SINGLE_SHOT), Power(1)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ATGWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATGWeaponBase::Shoot(AActor* target)
{
}
