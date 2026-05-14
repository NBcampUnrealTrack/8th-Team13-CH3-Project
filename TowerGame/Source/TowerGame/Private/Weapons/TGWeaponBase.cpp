// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/TGWeaponBase.h"

void UTGWeaponBase::Shoot(AActor* Instigator, class UMeshComponent* WeaponComponent, FVector MuzzlePos, FVector Direction, float Distance)
{
	QueryParams.bTraceComplex = true;
	Instigator = { Instigator };
	QueryParams.AddIgnoredActors(IgnoredActors);
}
