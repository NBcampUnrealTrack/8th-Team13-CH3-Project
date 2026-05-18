// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/TGWeaponShotgun.h"
#include "Enemies/TGBossBase.h"
#include "Enemies/TGEnemyBase.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"

void UTGWeaponShotgun::Shoot(AActor* Instigator, class UMeshComponent* WeaponComponent, FVector MuzzlePos, FVector Direction, float Distance, bool TriggerLock)
{
	if (!CanFire || TriggerLock)
		return;

	CanFire = false;
	GetWorld()->GetTimerManager().SetTimer(TimerFireDelay, this, &UTGWeaponBase::HandleFireDelay, status.ShotInterval, false);
	Super::Shoot(Instigator, WeaponComponent, MuzzlePos, Direction, Distance, TriggerLock);

	// 발포 이펙트
	SpawnAttachedEffects(status.Asset.FireParticle, WeaponComponent, MuzzlePos, status.Asset.FireParticleScale);

	for (int i = 0; i < status.Pellet; i++)
	{
		FVector SpreadDir = FMath::VRandCone(Direction, FMath::DegreesToRadians(status.BulletSpread));	//탄퍼짐 각도
		LineTrace(MuzzlePos, SpreadDir, Distance);
		if (TraceHit.bBlockingHit)
		{
			// 착탄 이펙트
			SpawnAttachedEffects(status.Asset.HitParticle, TraceHit.GetActor()->GetRootComponent(), TraceHit.Location, status.Asset.HitParticleScale, true);

			AActor* HitActor = TraceHit.GetActor();
			if (ATGEnemyBase* Enemy = Cast<ATGEnemyBase>(HitActor))
			{
				UGameplayStatics::ApplyDamage(Enemy, status.Power, Instigator->GetInstigatorController(), Instigator, nullptr);
			}
			else if (ATGBossBase* Boss = Cast<ATGBossBase>(HitActor))
			{
				UGameplayStatics::ApplyDamage(Boss, status.Power, Instigator->GetInstigatorController(), Instigator, nullptr);
			}
		}
	}
}
