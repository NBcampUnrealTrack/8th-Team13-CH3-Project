// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Phase/TGSentinelBossPhase1.h"

#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "TGMountedTower.h"
#include "Enemies/TGBossBase.h"
#include "Enemies/TGCoreBase.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Player/TGPlayer.h"

UTGSentinelBossPhase1::UTGSentinelBossPhase1() :
	WarningRadius(300.f),
	WarningDrawTime(1.5f),
	AttackDelay(1.5),
	AttackDamage(10),
	PendingAttackLocation(FVector::ZeroVector)
{
	// Phase 파괴 가능 Part 설정
	FTGBossBreakablePartData LeftArmPart;
	LeftArmPart.PartTag = TEXT("L_Arm");
	LeftArmPart.HPRatio = 0.1f;
	LeftArmPart.DestroyBonusDamageRatio = 0.05f;
	BreakableParts.Add(LeftArmPart);

	FTGBossBreakablePartData RightArmPart;
	RightArmPart.PartTag = TEXT("R_Arm");
	RightArmPart.HPRatio = 0.1f;
	RightArmPart.DestroyBonusDamageRatio = 0.05f;
	BreakableParts.Add(RightArmPart);

	static ConstructorHelpers::FObjectFinder<USoundBase> AttackSoundAsset(TEXT("/Game/Enemies/Sound/Enemy_Shot_00.Enemy_Shot_00"));
	if (AttackSoundAsset.Succeeded()){
		AttackSound = AttackSoundAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> AttackEffectAsset(
		TEXT("/Game/Particle/P_Explosion.P_Explosion")
	);

	if (AttackEffectAsset.Succeeded()){
		AttackEffect = AttackEffectAsset.Object;
	}
}

void UTGSentinelBossPhase1::EnterPhase()
{
	Super::EnterPhase();
	UE_LOG(LogTemp, Warning, TEXT("Sentinel Boss Phase 1 Enter"));
}

void UTGSentinelBossPhase1::ExitPhase()
{
	UE_LOG(LogTemp, Warning, TEXT("Sentinel Boss Phase 1 Exit"));
	Super::ExitPhase();
}

void UTGSentinelBossPhase1::ExecutePattern()
{
	if (!OwnerBoss) return;

	ATGPlayer* Player = OwnerBoss->GetPlayer();
	if (!Player) return;

	UWorld* World = OwnerBoss->GetWorld();
	if (!World) return;

	// Character 기준 바닥 근처로 보정해서 경고 범위를 표시한다.
	const float CapsuleHalfHeight = Player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	PendingAttackLocation = Player->GetActorLocation();
	PendingAttackLocation.Z -= CapsuleHalfHeight;
	DrawDebugCylinder(
		World,
		PendingAttackLocation,
		PendingAttackLocation + FVector(0.f, 0.f, 40.f),
		WarningRadius,
		48,
		FColor::Red,
		false,
		WarningDrawTime,
		0,
		4.f
	);

	FTimerHandle AttackDelayTimerHandle;
	World->GetTimerManager().SetTimer(
		AttackDelayTimerHandle,
		this,
		&UTGSentinelBossPhase1::ExecuteDelayedAttack,
		AttackDelay,
		false
	);
}

void UTGSentinelBossPhase1::ExecuteDelayedAttack()
{
	UWorld* World = OwnerBoss->GetWorld();
	if (!World) return;

	TArray<AActor*> OverlapActors;
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(OwnerBoss);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

	// 데미지를 줄 대상을 찾기 위한 Overlap Actor 탐색
	UKismetSystemLibrary::SphereOverlapActors(
		World,
		PendingAttackLocation,
		WarningRadius,
		ObjectTypes,
		nullptr,
		IgnoreActors,
		OverlapActors
	);

	PlayAttackSoundAtLocation(PendingAttackLocation);
	SpawnAttackEffectAtLocation(PendingAttackLocation);

	for (AActor* OverlapActor : OverlapActors){
		if (!OverlapActor) continue;

		if (!Cast<ATGPlayer>(OverlapActor) && !Cast<ATGCoreBase>(OverlapActor) && !Cast<ATGMountedTower>(OverlapActor)){
			continue;
		}

		UGameplayStatics::ApplyDamage(
			OverlapActor,
			AttackDamage,
			nullptr,
			OwnerBoss,
			nullptr
		);

		UE_LOG(LogTemp, Error, TEXT("Apply Damage %s"), *OverlapActor->GetName());
	}
}
