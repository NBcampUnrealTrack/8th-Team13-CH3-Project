// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/TGWeaponRepeater.h"
#include "Player/TGPlayer.h"
#include "Enemies/TGBossBase.h"
#include "Enemies/TGEnemyBase.h"
#include "Enemies/TGMissile.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"

void UTGWeaponRepeater::Tick(float DeltaTime)
{
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, FString::Printf(TEXT("SpreadTime: %f/%f"), CurSpreadTime, status.MaxSpreadTime));
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

bool UTGWeaponRepeater::IsTickableInEditor() const
{
	return false;
}

bool UTGWeaponRepeater::IsTickableWhenPaused() const
{
	return false;
}

TStatId UTGWeaponRepeater::GetStatId() const
{
	return TStatId();
}

UWorld* UTGWeaponRepeater::GetWorld() const
{
	return GetOuter()->GetWorld();
}

void UTGWeaponRepeater::Shoot(ATGPlayer* Instigator, class UMeshComponent* WeaponComponent, FVector MuzzlePos, FVector Direction, float Distance, bool TriggerLock)
{
	IsFire = true;
	CurSpreadTime += GetWorld()->GetDeltaSeconds();
	if (CurSpreadTime > status.MaxSpreadTime)
		CurSpreadTime = status.MaxSpreadTime;

	if (!CanFire || TriggerLock)
		return;

	CanFire = false;
	GetWorld()->GetTimerManager().SetTimer(TimerFireDelay, this, &UTGWeaponBase::HandleFireDelay, status.ShotInterval, false);
	Super::Shoot(Instigator, WeaponComponent, MuzzlePos, Direction, Distance, TriggerLock);

	// 발포 이펙트
	SpawnAttachedNiagaraEffects(status.Asset.FireParticle, WeaponComponent, MuzzlePos, status.Asset.FireParticleScale);
	UGameplayStatics::SpawnSoundAttached(
		status.Asset.FireSound,
		WeaponComponent,
		NAME_None,
		MuzzlePos,
		EAttachLocation::KeepWorldPosition,
		false,
		GetAsset()->FireSoundScale
	);
	Instigator->PlayRecoil(status.ShotInterval, status.Asset.RecoilInputScale);

	FVector SpreadDir = FMath::VRandCone(Direction, FMath::DegreesToRadians(FMath::Lerp(0.0f,status.MaxBulletSpread,(CurSpreadTime/status.MaxSpreadTime))));	//탄퍼짐 각도
	LineTrace(MuzzlePos, SpreadDir, Distance);
	//	레이저 이펙트
	FVector BeamEnd = TraceHit.bBlockingHit ? TraceHit.Location : MuzzlePos + Direction * Distance;
	SpawnBeamEffect(MuzzlePos, BeamEnd);

	if (TraceHit.bBlockingHit)
	{
		// 착탄 이펙트
		SpawnAttachedEffects(status.Asset.HitParticle, TraceHit.GetActor()->GetRootComponent(), TraceHit.Location, status.Asset.HitParticleScale, true);
		UGameplayStatics::SpawnSoundAttached(
			status.Asset.HitSound,
			TraceHit.GetActor()->GetRootComponent(),
			NAME_None,
			TraceHit.Location,
			EAttachLocation::KeepWorldPosition,
			false,
			GetAsset()->HitSoundScale, 1.0f, 0.0f, status.Asset.HitSoundAttenuation
		);
		AActor* HitActor = TraceHit.GetActor();

		if (Cast<ATGEnemyBase>(HitActor) || Cast<ATGMissile>(HitActor))
		{
			UGameplayStatics::ApplyDamage(HitActor, status.Power, Instigator->GetInstigatorController(), Instigator, nullptr);
		}
		else if (ATGBossBase* Boss = Cast<ATGBossBase>(HitActor))
		{
			UGameplayStatics::ApplyPointDamage(
				Boss,
				status.Power,
				SpreadDir,
				TraceHit,
				Instigator->GetInstigatorController(),
				Instigator,
				nullptr
			);
		}
	}
}
