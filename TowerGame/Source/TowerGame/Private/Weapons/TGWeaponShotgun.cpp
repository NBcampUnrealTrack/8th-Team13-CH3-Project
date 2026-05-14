// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/TGWeaponShotgun.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Enemies/TGEnemyBase.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

void UTGWeaponShotgun::Shoot(AActor* Instigator, class UMeshComponent* WeaponComponent, FVector MuzzlePos, FVector Direction, float Distance)
{
	Super::Shoot(Instigator, WeaponComponent, MuzzlePos, Direction, Distance);
}
