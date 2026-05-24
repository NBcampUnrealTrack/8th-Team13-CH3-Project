// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Pattern/TGSphereAreaPattern.h"

#include "Enemies/TGBossBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Particles/ParticleSystem.h"
#include "Player/TGPlayer.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"

UTGSphereAreaPattern::UTGSphereAreaPattern()
{
	static ConstructorHelpers::FObjectFinder<USoundBase> AttackSoundAsset(
		TEXT("/Game/Enemies/Sound/Enemy_Shot_00.Enemy_Shot_00"));
	if (AttackSoundAsset.Succeeded()){
		AttackSound = AttackSoundAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> AttackEffectAsset(
		TEXT("/Game/Particle/P_Explosion.P_Explosion"));
	if (AttackEffectAsset.Succeeded()){
		AttackEffect = AttackEffectAsset.Object;
	}
}

void UTGSphereAreaPattern::GetAttackLocation()
{
	AttackLocation = FVector::ZeroVector;

	if (!OwnerBoss) return;

	ATGPlayer* Player = OwnerBoss->GetPlayer();
	if (!Player) return;

	AttackLocation = Player->GetActorLocation();
}

void UTGSphereAreaPattern::DrawWarning(float WarningDrawTime)
{
	SpawnSphereWarning(AttackLocation, WarningRadius, WarningDrawTime);
}

void UTGSphereAreaPattern::CollectDamageTargets(TArray<AActor*>& OutTargets) const
{
	UWorld* World = nullptr;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TArray<AActor*> IgnoreActors;

	if (!PrepareOverlapQuery(World, ObjectTypes, IgnoreActors)) return;

	UKismetSystemLibrary::SphereOverlapActors(
		World,
		AttackLocation,
		WarningRadius,
		ObjectTypes,
		nullptr,
		IgnoreActors,
		OutTargets
	);
}
