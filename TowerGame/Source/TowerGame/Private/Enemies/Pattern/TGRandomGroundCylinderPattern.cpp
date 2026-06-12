// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Pattern/TGRandomGroundCylinderPattern.h"

#include "Enemies/TGBossBase.h"
#include "Kismet/KismetSystemLibrary.h"

UTGRandomGroundCylinderPattern::UTGRandomGroundCylinderPattern() :
	CylinderHeight(100.f),
	RandomAreaRadius(1200.f),
	AttackCount(15)
{
	// 공격 사운드/이펙트 기본값은 UTGPatternBase 생성자에서 지정
}

void UTGRandomGroundCylinderPattern::StartPattern(float WarningDrawTime)
{
	StopPattern();
	SpawnWarnings(WarningDrawTime);
}

void UTGRandomGroundCylinderPattern::StopPattern()
{
	if (!OwnerBoss) return;

	UWorld* World = OwnerBoss->GetWorld();
	if (!World) return;

	// Timer 정리
	for (FTimerHandle& TimerHandle : AttackTimerHandles){
		World->GetTimerManager().ClearTimer(TimerHandle);
	}

	// 사용한 배열 정리
	AttackTimerHandles.Empty();
}

void UTGRandomGroundCylinderPattern::SpawnWarnings(float WarningDrawTime)
{
	if (!OwnerBoss) return;

	UWorld* World = OwnerBoss->GetWorld();
	if (!World) return;

	// Boss 위치 & 제외 반경
	const FVector CenterLocation = OwnerBoss->GetActorLocation();
	const float ExcludeRadius = OwnerBoss->GetSpawnClearRadius();

	if (RandomAreaRadius < ExcludeRadius) return;

	for (int32 Index = 0; Index < AttackCount; ++Index){
		FVector2D RandomPoint = FVector2D::ZeroVector;

		// 제외 반경 ~ 생성 범위 내 랜덤 위치 생성
		do{
			RandomPoint = FMath::RandPointInCircle(RandomAreaRadius);
		} while (RandomPoint.Size() < ExcludeRadius);

		// 경고 생성 위치 지정
		const FVector WarningLocation = CenterLocation + FVector(RandomPoint.X, RandomPoint.Y, 0.f);

		// 경고 범위 생성
		SpawnCircleWarning(WarningLocation, WarningRadius, WarningDrawTime);

		// 공격 타이머 델리게이트 받을 함수 생성
		FTimerHandle AttackTimerHandle;
		FTimerDelegate AttackTimerDelegate;
		if (Index == AttackCount - 1){
			// 마지막 공격
			AttackTimerDelegate.BindLambda([this, WarningLocation]()
			{
				ExecuteAttackLocation(WarningLocation);
				OnPatternFinished.Broadcast();
			});
		}
		else{
			AttackTimerDelegate.BindUObject(
				this,
				&UTGRandomGroundCylinderPattern::ExecuteAttackLocation,
				WarningLocation
			);
		}

		// 공격 타이머
		World->GetTimerManager().SetTimer(
			AttackTimerHandle,
			AttackTimerDelegate,
			WarningDrawTime,
			false
		);

		AttackTimerHandles.Add(AttackTimerHandle);
	}
}

void UTGRandomGroundCylinderPattern::ExecuteAttackLocation(FVector InAttackLocation)
{
	// 공격 대상 탐색
	TArray<AActor*> DamageTargets;
	CollectCylinderTargets(DamageTargets, InAttackLocation);

	// VFX & 데미지 적용
	PlayAttackFeedback(InAttackLocation);
	ApplyDamageToTargets(DamageTargets, AttackDamage);
}

void UTGRandomGroundCylinderPattern::CollectCylinderTargets(
	TArray<AActor*>& OutTargets,
	const FVector& InAttackLocation) const
{
	UWorld* World = nullptr;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TArray<AActor*> IgnoreActors;

	// 탐색 대상 설정
	if (!PrepareOverlapQuery(World, ObjectTypes, IgnoreActors)) return;

	TArray<AActor*> CandidateTargets;

	// 공격 범위 설정
	const FVector BoxCenter =InAttackLocation + FVector(0.f, 0.f, CylinderHeight * 0.5f);
	const FVector BoxExtent = FVector(WarningRadius, WarningRadius, CylinderHeight * 0.5f);

	// 범위 내 Actor 탐색
	UKismetSystemLibrary::BoxOverlapActors(
		World,
		BoxCenter,
		BoxExtent,
		ObjectTypes,
		nullptr,
		IgnoreActors,
		CandidateTargets
	);

	// 박스 범위 -> 원기둥 범위 내 탐색
	for (AActor* CandidateTarget : CandidateTargets){
		if (!CandidateTarget) continue;

		const float Distance2D = FVector::Dist2D(
			InAttackLocation,
			CandidateTarget->GetActorLocation()
		);

		if (Distance2D > WarningRadius) continue;

		OutTargets.Add(CandidateTarget);
	}
}
