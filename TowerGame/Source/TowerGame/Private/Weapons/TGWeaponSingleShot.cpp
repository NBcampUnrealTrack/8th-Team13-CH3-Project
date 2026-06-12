// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/TGWeaponSingleShot.h"

void UTGWeaponSingleShot::Tick(float DeltaTime)
{
	IsPrevFire = IsFire;
	IsFire = false;
}

bool UTGWeaponSingleShot::IsTickable() const
{
	return IsFire || IsPrevFire;
}

void UTGWeaponSingleShot::Shoot(ATGPlayer* Instigator, class UMeshComponent* WeaponComponent, FVector MuzzlePos, FVector Direction, float Distance, bool TriggerLock)
{
	IsFire = true;
	if (!CanFire || IsPrevFire || TriggerLock)
		return;

	StartFireCooldown(status.ShotCoolTime);
	Super::Shoot(Instigator, WeaponComponent, MuzzlePos, Direction, Distance, TriggerLock);

	PlayMuzzleFeedback(Instigator, WeaponComponent, MuzzlePos, status.ShotCoolTime);
	FireSingleTrace(Instigator, MuzzlePos, Direction, Distance, status.Power);
}
