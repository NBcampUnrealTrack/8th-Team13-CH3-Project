// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/TGWeaponRepeater.h"

void UTGWeaponRepeater::Tick(float DeltaTime)
{
	if (IsFire)
		IsFire = false;
	else
	{
		CurSpreadTime -= status.MaxSpreadTime * DeltaTime * 2.0f;
		if (CurSpreadTime <= 0.0f)
			CurSpreadTime = 0.0f;
	}
}

bool UTGWeaponRepeater::IsTickable() const
{
	return CurSpreadTime > 0.0f;
}

void UTGWeaponRepeater::Shoot(ATGPlayer* Instigator, class UMeshComponent* WeaponComponent, FVector MuzzlePos, FVector Direction, float Distance, bool TriggerLock)
{
	IsFire = true;
	CurSpreadTime += GetWorld()->GetDeltaSeconds();
	if (CurSpreadTime > status.MaxSpreadTime)
		CurSpreadTime = status.MaxSpreadTime;

	if (!CanFire || TriggerLock)
		return;

	StartFireCooldown(status.ShotInterval);
	Super::Shoot(Instigator, WeaponComponent, MuzzlePos, Direction, Distance, TriggerLock);

	PlayMuzzleFeedback(Instigator, WeaponComponent, MuzzlePos, status.ShotInterval);

	//	탄퍼짐 각도: 연사 지속시간에 비례해 최대치까지 증가
	const FVector SpreadDir = FMath::VRandCone(Direction, FMath::DegreesToRadians(FMath::Lerp(0.0f, status.MaxBulletSpread, CurSpreadTime / status.MaxSpreadTime)));
	FireSingleTrace(Instigator, MuzzlePos, SpreadDir, Distance, status.Power);
}
