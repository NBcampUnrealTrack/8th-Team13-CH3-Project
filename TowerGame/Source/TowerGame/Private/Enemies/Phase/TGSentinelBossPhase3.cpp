// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Phase/TGSentinelBossPhase3.h"

#include "TGMountedTower.h"
#include "Enemies/TGBossBase.h"
#include "Enemies/TGCoreBase.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Player/TGPlayer.h"

UTGSentinelBossPhase3::UTGSentinelBossPhase3() :
	WarningRadius(250.f),
	WarningDrawTime(1.5f),
	AttackDelay(1.5f),
	AttackDamage(20.f),
	PendingAttackLocation(FVector::ZeroVector)
{
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

void UTGSentinelBossPhase3::EnterPhase()
{
	Super::EnterPhase();
	UE_LOG(LogTemp, Warning, TEXT("Sentinel Boss Phase 3 Enter"));
}

void UTGSentinelBossPhase3::ExitPhase()
{
	UE_LOG(LogTemp, Warning, TEXT("Sentinel Boss Phase 3 Exit"));
	Super::ExitPhase();
}

void UTGSentinelBossPhase3::ExecutePattern()
{
	if (!OwnerBoss) return;

	ATGPlayer* Player = OwnerBoss->GetPlayer();
	if (!Player) return;

	UWorld* World = OwnerBoss->GetWorld();
	if (!World) return;

	PendingAttackLocation = Player->GetActorLocation();

	// PlayerHalfHeight 기준 구형 범위 생성
	DrawDebugSphere(
		World,
		PendingAttackLocation,
		WarningRadius,
		32,
		FColor::Red,
		false,
		WarningDrawTime,
		0,
		1.f
	);

	FTimerHandle AttackDelayTimerHandle;
	World->GetTimerManager().SetTimer(
		AttackDelayTimerHandle,
		this,
		&UTGSentinelBossPhase3::ExecuteDelayedAttack,
		AttackDelay,
		false
	);

	UE_LOG(LogTemp, Warning, TEXT("Sentinel Boss Phase 3 Pattern"));
}

void UTGSentinelBossPhase3::ExecuteDelayedAttack()
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

		// Player, Core, MountedTowet만 데미지 적용
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
