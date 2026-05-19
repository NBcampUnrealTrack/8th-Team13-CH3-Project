// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Phase/TGSentinelBossPhase2.h"

#include "TGMountedTower.h"
#include "Enemies/TGBossBase.h"
#include "Enemies/TGCoreBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/TGPlayer.h"

UTGSentinelBossPhase2::UTGSentinelBossPhase2() :
	WarningRadius(150.f),
	WarningDrawTime(1.5f),
	AttackDelay(1.5f),
	AttackDamage(15.f),
	PendingAttackLocation(FVector::ZeroVector)
{
	// Phase 파괴 가능 Part 설정
	FTGBossBreakablePartData BodyPart;
	BodyPart.PartTag = TEXT("Body");
	BodyPart.HPRatio = 0.15f;
	BodyPart.DestroyBonusDamageRatio = 0.05f;
	BreakableParts.Add(BodyPart);
}

void UTGSentinelBossPhase2::EnterPhase()
{
	Super::EnterPhase();
	UE_LOG(LogTemp, Warning, TEXT("Sentinel Boss Phase 2 Enter"));
}

void UTGSentinelBossPhase2::ExitPhase()
{
	UE_LOG(LogTemp, Warning, TEXT("Sentinel Boss Phase 2 Exit"));
	Super::ExitPhase();
}

void UTGSentinelBossPhase2::ExecutePattern()
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
		&UTGSentinelBossPhase2::ExecuteDelayedAttack,
		AttackDelay,
		false
	);

	UE_LOG(LogTemp, Warning, TEXT("Sentinel Boss Phase 2 Pattern"));
}

void UTGSentinelBossPhase2::ExecuteDelayedAttack()
{
	UWorld* World = OwnerBoss->GetWorld();
	if (!World) return;

	TArray<AActor*> OverlapActors;
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(OwnerBoss);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

	UKismetSystemLibrary::SphereOverlapActors(
		World,
		PendingAttackLocation,
		WarningRadius,
		ObjectTypes,
		nullptr,
		IgnoreActors,
		OverlapActors
	);

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
