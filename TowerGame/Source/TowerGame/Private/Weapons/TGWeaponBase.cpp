// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/TGWeaponBase.h"
#include "Kismet/GameplayStatics.h"
//#include "Kismet/KismetSystemLibrary.h"
#include "Particles/ParticleSystem.h"
#include "Components/DecalComponent.h"

void UTGWeaponBase::Shoot(AActor* Instigator, class UMeshComponent* WeaponComponent, FVector MuzzlePos, FVector Direction, float Distance, bool TriggerLock)
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

bool UTGWeaponBase::LineTrace(FVector MuzzlePos, FVector Direction, float Distance)
{
	return GetWorld()->LineTraceSingleByChannel(
		TraceHit,
		MuzzlePos,
		MuzzlePos + Direction * Distance,
		ECC_Visibility,
		QueryParams
	);

	//return UKismetSystemLibrary::LineTraceSingle(
	//	GetWorld(), //어느 월드의 소속인가? (this)를 넣어줘도 됨
	//	MuzzlePos,
	//	MuzzlePos + Direction * Distance,
	//	UEngineTypes::ConvertToTraceType(ECC_Visibility),	// 사용할 트레이스채널
	//	QueryParams.bTraceComplex,	// 복합콜리전 사용
	//	IgnoredActors,	// 해당 액터는 이 트레이스를 무시
	//	EDrawDebugTrace::ForDuration,	//디버그(그리기 타입 적용),
	//	TraceHit,
	//	true,	// 자기자신을 Ignore
	//	FLinearColor::Blue,	//디버그 색깔
	//	FLinearColor::Yellow,	//트레이스 히트 시 색깔
	//	5.0f
	//);
}
