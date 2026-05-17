// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/TGWeaponBase.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Engine/DecalActor.h"

void UTGWeaponBase::Shoot(AActor* Instigator, class UMeshComponent* WeaponComponent, FVector MuzzlePos, FVector Direction, float Distance)
{
	QueryParams.bTraceComplex = true;
	QueryParams.ClearIgnoredSourceObjects();
	IgnoredActors = { Instigator };
	QueryParams.AddIgnoredActors(IgnoredActors);
}

void UTGWeaponBase::SpawnAttachedEffects(UParticleSystem* particle, USceneComponent* AttachToComponent, FVector Location, float ParticleScale, bool bSpawnDecal, float DecalScale)
{
	UParticleSystemComponent* FireParticle = UGameplayStatics::SpawnEmitterAttached(
		GetAsset()->FireParticle,
		AttachToComponent,
		NAME_None,
		Location,
		FRotator::ZeroRotator,
		FVector::OneVector * GetAsset()->FireParticleScale,
		EAttachLocation::KeepWorldPosition
	);
}

void UTGWeaponBase::HandleFireDelay()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerFireDelay);
	CanFire = true;
}
