// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Pattern/TGGroundCylinderPattern.h"

#include "Components/CapsuleComponent.h"
#include "Enemies/TGBossBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/TGPlayer.h"

UTGGroundCylinderPattern::UTGGroundCylinderPattern() : CylinderHeight(100)
{
	// 공격 사운드/이펙트 기본값은 UTGPatternBase 생성자에서 지정
}

void UTGGroundCylinderPattern::StartPattern(float WarningDrawTime)
{
	StopPattern();

	// 공격 범위 경고 표시
	GetAttackLocation();
	SpawnCircleWarning(AttackLocation, WarningRadius, WarningDrawTime);

	if (!OwnerBoss) return;

	UWorld* World = OwnerBoss->GetWorld();
	if (!World) return;

	// 지연 시간 후 공격 판정
	World->GetTimerManager().SetTimer(
		AttackDelayTimerHandle,
		this,
		&UTGGroundCylinderPattern::ExecuteAttack,
		WarningDrawTime,
		false
	);
}

void UTGGroundCylinderPattern::StopPattern()
{
	if (!OwnerBoss) return;

	UWorld* World = OwnerBoss->GetWorld();
	if (!World) return;

	// Pattern Timer 정리
	World->GetTimerManager().ClearTimer(AttackDelayTimerHandle);
}

void UTGGroundCylinderPattern::ExecuteAttack()
{
	Super::ExecuteAttack();
	OnPatternFinished.Broadcast();
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
