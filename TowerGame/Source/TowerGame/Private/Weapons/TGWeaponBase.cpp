// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/TGWeaponBase.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Components/DecalComponent.h"

void UTGWeaponBase::Shoot(AActor* Instigator, class UMeshComponent* WeaponComponent, FVector MuzzlePos, FVector Direction, float Distance)
{
	QueryParams.bTraceComplex = true;
	QueryParams.ClearIgnoredSourceObjects();
	IgnoredActors = { Instigator };
	QueryParams.AddIgnoredActors(IgnoredActors);
}

void UTGWeaponBase::SpawnAttachedEffects(UParticleSystem* Particle, USceneComponent* AttachToComponent, FVector Location, float ParticleScale, bool bSpawnDecal, float DecalLifeSpan)
{
	UGameplayStatics::SpawnEmitterAttached(
		Particle,
		AttachToComponent,
		NAME_None,
		Location,
		FRotator::ZeroRotator,
		FVector::OneVector * ParticleScale,
		EAttachLocation::KeepWorldPosition
	);

	if (bSpawnDecal)
	{
		UDecalComponent* decal = UGameplayStatics::SpawnDecalAttached(
			GetAsset()->BulletMarks,
			FVector::OneVector * GetAsset()->BulletMarksScale,
			AttachToComponent,
			NAME_None,
			Location,
			FRotationMatrix::MakeFromXZ(TraceHit.ImpactNormal, FMath::VRand()).Rotator(),
			EAttachLocation::KeepWorldPosition,
			DecalLifeSpan
		);
		decal->RegisterComponent();
	}
}

void UTGWeaponBase::HandleFireDelay()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerFireDelay);
	CanFire = true;
}
