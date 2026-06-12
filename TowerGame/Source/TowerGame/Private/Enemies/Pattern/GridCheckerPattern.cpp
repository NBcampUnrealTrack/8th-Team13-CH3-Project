// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Pattern/GridCheckerPattern.h"

#include "Core/Grid/TGGridBase.h"
#include "Enemies/TGBossBase.h"
#include "Enemies/TGWaveManager.h"
#include "Kismet/KismetSystemLibrary.h"

UGridCheckerPattern::UGridCheckerPattern() :
	CylinderHeight(100.f),
	GridWarningInterval(0.01f)
{
	// 공격 사운드/이펙트 기본값은 UTGPatternBase 생성자에서 지정
}

void UGridCheckerPattern::StartPattern(float WarningDrawTime)
{
	StopPattern();

	ATGWaveManager* WaveManager = ATGWaveManager::Get(OwnerBoss);
	if (!WaveManager) return;

	ATGGridBase* GridBase = WaveManager->FindGridBase();
	if (!GridBase) return;

	QueueGridWarnings(GridBase, WarningDrawTime);
}

void UGridCheckerPattern::StopPattern()
{
	if (!OwnerBoss) return;

	UWorld* World = OwnerBoss->GetWorld();
	if (!World) return;

	// 경고 생성 Timer 정리
	for (FTimerHandle& TimerHandle : WarningTimerHandles){
		World->GetTimerManager().ClearTimer(TimerHandle);
	}

	// 공격 Timer 정리
	for (FTimerHandle& TimerHandle : AttackTimerHandles){
		World->GetTimerManager().ClearTimer(TimerHandle);
	}

	WarningTimerHandles.Empty();
	AttackTimerHandles.Empty();
}

void UGridCheckerPattern::QueueGridWarnings(ATGGridBase* GridBase, float WarningDrawTime)
{
	if (!OwnerBoss || !GridBase) return;

	UWorld* World = OwnerBoss->GetWorld();
	if (!World) return;

	const int32 GridX = GridBase->GetGridX();
	const int32 GridY = GridBase->GetGridY();
	if (GridX <= 0 || GridY <= 0) return;

	// 방문 처리에 사용할 배열
	TArray<TArray<bool>> bVisited;
	bVisited.SetNum(GridY);
	for (int32 Y = 0; Y < GridY; ++Y){
		bVisited[Y].Init(false, GridX);
	}

	// 어디 그리드를 범위로 지정할지 랜덤으로 선택
	const int32 SelectedParity = FMath::RandRange(0, 1);
	const int32 DirectionX[] = { 1, -1, 0, 0 };
	const int32 DirectionY[] = { 0, 0, 1, -1 };

	// BFS
	TArray<FIntPoint> Queue;
	Queue.Add(FIntPoint(0, 0));
	bVisited[0][0] = true;

	TArray<FIntPoint> WarningPoints;

	for (int32 Head = 0; Head < Queue.Num(); ++Head){
		const FIntPoint CurrentPoint = Queue[Head];

		// 경고 범위 판별
		if ((CurrentPoint.X + CurrentPoint.Y) % 2 == SelectedParity){
			WarningPoints.Add(CurrentPoint);
		}

		for (int32 DirectionIndex = 0; DirectionIndex < 4; ++DirectionIndex){
			const int32 NextX = CurrentPoint.X + DirectionX[DirectionIndex];
			const int32 NextY = CurrentPoint.Y + DirectionY[DirectionIndex];

			if (NextX < 0 || NextX >= GridX || NextY < 0 || NextY >= GridY) continue;
			if (bVisited[NextY][NextX]) continue;

			bVisited[NextY][NextX] = true;
			Queue.Add(FIntPoint(NextX, NextY));
		}
	}

	if (WarningPoints.IsEmpty()){
		OnPatternFinished.Broadcast();
		return;
	}

	for (int32 Index = 0; Index < WarningPoints.Num(); ++Index){
		const FIntPoint WarningPoint = WarningPoints[Index];
		const bool bFinishPattern = Index == WarningPoints.Num() - 1;
		const float WarningDelay = Index * GridWarningInterval;

		// 첫 범위 지정
		if (WarningDelay <= 0.f){
			SpawnWarningAtGridPoint(GridBase, WarningPoint, WarningDrawTime, bFinishPattern);
			continue;
		}

		// WarningDelay을 사용해 차례로 위험 범위 지정
		FTimerHandle WarningTimerHandle;
		World->GetTimerManager().SetTimer(
			WarningTimerHandle,
			FTimerDelegate::CreateWeakLambda(this, [this, GridBase, WarningPoint, WarningDrawTime, bFinishPattern]()
			{
				SpawnWarningAtGridPoint(GridBase, WarningPoint, WarningDrawTime, bFinishPattern);
			}),
			WarningDelay,
			false
		);

		WarningTimerHandles.Add(WarningTimerHandle);
	}
}

void UGridCheckerPattern::SpawnWarningAtGridPoint(
	ATGGridBase* GridBase,
	FIntPoint GridPoint,
	float WarningDrawTime,
	bool bFinishPattern)
{
	if (!OwnerBoss || !GridBase) return;
	// 해당 위치에 Tower가 설치되어 있는지 확인
	if (GridBase->IsTowerPlacedInPosition(GridPoint)){
		if (bFinishPattern){
			OnPatternFinished.Broadcast();
		}
		return;
	}

	UWorld* World = OwnerBoss->GetWorld();
	if (!World) return;

	// GridPoint을 월드 좌표로 변환
	const FVector WarningLocation = GridBase->ConvertIndexToVector(GridPoint);
	SpawnCircleWarning(WarningLocation, WarningRadius, WarningDrawTime);

	// 공격 예약
	FTimerHandle AttackTimerHandle;
	World->GetTimerManager().SetTimer(
		AttackTimerHandle,
		FTimerDelegate::CreateWeakLambda(this, [this, GridBase, GridPoint, WarningLocation, bFinishPattern]()
		{
			ExecuteAttackAtGridPoint(GridBase, GridPoint, WarningLocation);
			if (bFinishPattern){
				OnPatternFinished.Broadcast();
			}
		}),
		WarningDrawTime,
		false
	);

	AttackTimerHandles.Add(AttackTimerHandle);
}

void UGridCheckerPattern::ExecuteAttackAtGridPoint(
	ATGGridBase* GridBase,
	FIntPoint GridPoint,
	const FVector& InAttackLocation)
{
	if (!GridBase) return;
	if (GridBase->IsTowerPlacedInPosition(GridPoint)) return;

	// 공격 대상 탐색
	TArray<AActor*> DamageTargets;
	CollectCylinderTargets(DamageTargets, InAttackLocation);

	// VFX & 데미지 적용
	PlayAttackFeedback(InAttackLocation, 0.7f);
	ApplyDamageToTargets(DamageTargets, AttackDamage);
}

void UGridCheckerPattern::CollectCylinderTargets(
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
	const FVector BoxCenter = InAttackLocation + FVector(0.f, 0.f, CylinderHeight * 0.5f);
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
