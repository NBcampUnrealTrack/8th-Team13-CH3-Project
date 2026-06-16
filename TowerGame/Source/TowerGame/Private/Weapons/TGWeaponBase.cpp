// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/TGWeaponBase.h"
#include "Player/TGPlayer.h"
#include "Enemies/TGBossBase.h"
#include "Enemies/TGEnemyBase.h"
#include "Enemies/TGMissile.h"
#include "Enemies/TGSwarmEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Particles/ParticleSystem.h"
#include "Components/DecalComponent.h"
#include "Curves/CurveVector.h"
#include "Components/TimelineComponent.h"

UWorld* UTGWeaponBase::GetWorld() const
{
	return GetOuter() ? GetOuter()->GetWorld() : nullptr;
}

void UTGWeaponBase::SpawnBeamEffect(FVector Start, FVector End)
{
	UNiagaraSystem* Beam = GetAsset()->BeamEffect;
	if (Beam == nullptr) return;

	UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(), Beam, FVector::ZeroVector);
	if (NiagaraComponent)
	{
		NiagaraComponent->SetVectorParameter(FName("BeamStart"), Start);
		NiagaraComponent->SetVectorParameter(FName("BeamEnd"), End);
	}
}

void UTGWeaponBase::Shoot(class ATGPlayer* Instigator, class UMeshComponent* WeaponComponent, FVector MuzzlePos,
	FVector Direction, float Distance, bool TriggerLock)
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

void UTGWeaponBase::SpawnAttachedNiagaraEffects(UNiagaraSystem* NiagaraSystem, USceneComponent* AttachToComponent, FVector Location, float Scale)
{
	if (!NiagaraSystem) return;

	UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
		NiagaraSystem,
		AttachToComponent,
		NAME_None,
		Location,
		AttachToComponent->GetComponentRotation(),
		EAttachLocation::KeepWorldPosition,
		true
	);
	if (NiagaraComp)
		NiagaraComp->SetWorldScale3D(FVector(Scale));
}

void UTGWeaponBase::HandleFireDelay()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerFireDelay);
	CanFire = true;
}

void UTGWeaponBase::StartFireCooldown(float Interval)
{
	CanFire = false;
	if (UWorld* World = GetWorld())
		World->GetTimerManager().SetTimer(TimerFireDelay, this, &UTGWeaponBase::HandleFireDelay, Interval, false);
}

void UTGWeaponBase::PlayMuzzleFeedback(ATGPlayer* Instigator, UMeshComponent* WeaponComponent, const FVector& MuzzlePos, float RecoilInterval)
{
	const FTGWeaponAsset* Asset = GetAsset();
	if (!Asset) return;

	// 발포 이펙트
	SpawnAttachedNiagaraEffects(Asset->FireParticle, WeaponComponent, MuzzlePos, Asset->FireParticleScale);
	UGameplayStatics::SpawnSoundAttached(
		Asset->FireSound,
		WeaponComponent,
		NAME_None,
		MuzzlePos,
		EAttachLocation::KeepWorldPosition,
		false,
		Asset->FireSoundScale
	);

	if (Instigator)
		Instigator->PlayRecoil(RecoilInterval, Asset->RecoilInputScale);
}

void UTGWeaponBase::FireSingleTrace(ATGPlayer* Instigator, const FVector& MuzzlePos, const FVector& Direction, float Distance, float Power)
{
	const FTGWeaponAsset* Asset = GetAsset();
	if (!Asset) return;

	LineTrace(MuzzlePos, Direction, Distance);

	//	레이저 이펙트
	const FVector BeamEnd = TraceHit.bBlockingHit ? TraceHit.Location : MuzzlePos + Direction * Distance;
	SpawnBeamEffect(MuzzlePos, BeamEnd);

	if (!TraceHit.bBlockingHit) return;

	AActor* HitActor = TraceHit.GetActor();
	if (!HitActor) return;

	// 착탄 이펙트
	SpawnAttachedEffects(Asset->HitParticle, HitActor->GetRootComponent(), TraceHit.Location, Asset->HitParticleScale, true);
	UGameplayStatics::SpawnSoundAttached(
		Asset->HitSound,
		HitActor->GetRootComponent(),
		NAME_None,
		TraceHit.Location,
		EAttachLocation::KeepWorldPosition,
		false,
		Asset->HitSoundScale, 1.0f, 0.0f, Asset->HitSoundAttenuation
	);

	// 데미지 적용 — PointDamage로 통일해 보스 부위판정(HitInfo)까지 한 경로로 처리
	if (Cast<ATGEnemyBase>(HitActor) || Cast<ATGMissile>(HitActor) || Cast<ATGBossBase>(HitActor) || Cast<ATGSwarmEnemy>(HitActor))
	{
		// 특성으로 강화된 무기 데미지 배수 반영
		const float FinalPower = Power * (Instigator ? Instigator->GetWeaponDamageMultiplier() : 1.f);

		UGameplayStatics::ApplyPointDamage(
			HitActor,
			FinalPower,
			Direction,
			TraceHit,
			Instigator ? Instigator->GetInstigatorController() : nullptr,
			Instigator,
			nullptr
		);
	}
}

bool UTGWeaponBase::LineTrace(FVector MuzzlePos, FVector Direction, float Distance)
{
	return GetWorld()->LineTraceSingleByChannel(
		TraceHit,
		MuzzlePos,
		MuzzlePos + Direction * Distance,
		ECC_Visibility,
		QueryParams
	);
}
