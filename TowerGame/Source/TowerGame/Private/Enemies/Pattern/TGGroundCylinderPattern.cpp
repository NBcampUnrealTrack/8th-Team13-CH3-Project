// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Pattern/TGGroundCylinderPattern.h"

#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Enemies/TGBossBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Particles/ParticleSystem.h"
#include "Player/TGPlayer.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"

UTGGroundCylinderPattern::UTGGroundCylinderPattern() : CylinderHeight(100)
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

void UTGGroundCylinderPattern::GetAttackLocation()
{
	AttackLocation = FVector::ZeroVector;

	if (!OwnerBoss) return;

	ATGPlayer* Player = OwnerBoss->GetPlayer();
	if (!Player) return;

	UCapsuleComponent* PlayerCapsule = Player->GetCapsuleComponent();
	if (!PlayerCapsule) return;

	const float CapsuleHalfHeight = PlayerCapsule->GetScaledCapsuleHalfHeight();

	// Player 위치에서 CapSuleHalfHeight 만큼 내려서 바닥 위치를 저장
	AttackLocation = Player->GetActorLocation();
	AttackLocation.Z -= CapsuleHalfHeight;
}

void UTGGroundCylinderPattern::DrawWarning(float WarningDrawTime)
{
	if (!OwnerBoss) return;

	UWorld* World = OwnerBoss->GetWorld();
	if (!World) return;

	// 경고 범위 Draw
	DrawDebugCylinder(
		World,
		AttackLocation,
		AttackLocation + FVector(0.f, 0.f, CylinderHeight),
		WarningRadius,
		48,
		FColor::Red,
		false,
		WarningDrawTime,
		0,
		4.f
	);
}

void UTGGroundCylinderPattern::CollectDamageTargets(TArray<AActor*>& OutTargets) const
{
	UWorld* World = nullptr;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TArray<AActor*> IgnoreActors;

	if (!PrepareOverlapQuery(World, ObjectTypes, IgnoreActors)) return;

	TArray<AActor*> Candidate;

	// 후보를 찾기 위한 BosOverlap
	const FVector BoxCenter = AttackLocation + FVector(0.f, 0.f, CylinderHeight * 0.5f);
	const FVector BoxExtent = FVector(WarningRadius, WarningRadius, CylinderHeight * 0.5f);

	// Box로 탐색
	UKismetSystemLibrary::BoxOverlapActors(
		World,
		BoxCenter,
		BoxExtent,
		ObjectTypes,
		nullptr,
		IgnoreActors,
		Candidate
	);

	// Dist2D로 박스 -> 원기둥 범위 내 후보만 필터링
	for (AActor* CandidateActor : Candidate){
		if (!CandidateActor) continue;

		const float Distance2D = FVector::Dist2D(
			AttackLocation, CandidateActor->GetActorLocation());

		if (Distance2D > WarningRadius) continue;

		OutTargets.Add(CandidateActor);
	}
}
