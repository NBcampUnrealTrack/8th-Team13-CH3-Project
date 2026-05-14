// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/TGWeaponBase.h"

void UTGWeaponBase::Shoot(AActor* Instigator, class UMeshComponent* WeaponComponent, FVector MuzzlePos, FVector Direction, float Distance)
{
	QueryParams.bTraceComplex = true;
	QueryParams.ClearIgnoredActors();
	IgnoredActors = { Instigator };
	QueryParams.AddIgnoredActors(IgnoredActors);
}

void UTGWeaponBase::HandleFireDelay()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerFireDelay);
	CanFire = true;
}
