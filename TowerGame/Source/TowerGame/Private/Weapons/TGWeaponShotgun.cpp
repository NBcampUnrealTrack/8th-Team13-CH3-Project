// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/TGWeaponShotgun.h"

void UTGWeaponShotgun::Shoot(ATGPlayer* Instigator, class UMeshComponent* WeaponComponent, FVector MuzzlePos, FVector Direction, float Distance, bool TriggerLock)
{
	if (!CanFire || TriggerLock)
		return;

	StartFireCooldown(status.ShotInterval);
	Super::Shoot(Instigator, WeaponComponent, MuzzlePos, Direction, Distance, TriggerLock);

	PlayMuzzleFeedback(Instigator, WeaponComponent, MuzzlePos, status.ShotInterval);

	for (int i = 0; i < status.Pellet; i++)
	{
		const FVector SpreadDir = FMath::VRandCone(Direction, FMath::DegreesToRadians(status.BulletSpread));	//탄퍼짐 각도
		FireSingleTrace(Instigator, MuzzlePos, SpreadDir, Distance, status.Power);
	}
}
