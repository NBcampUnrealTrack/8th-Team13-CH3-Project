// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseTower/DebuffTower.h"
#include "Enemies/TGNavigationManager.h"
#include "Enemies/TGTargetable.h"

ATGDebuffTower::ATGDebuffTower()
{
	PrimaryActorTick.bCanEverTick = true;

	//	기본적으로 인터랙션을 끕니다
	SetInteractionEnabled(false);
}

void ATGDebuffTower::BeginPlay()
{
	Super::BeginPlay();

	SetRangeSphereScale(DebuffRange);
}

void ATGDebuffTower::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopDebuff();
	Super::EndPlay(EndPlayReason);
}

void ATGDebuffTower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//	적 탐지 (무기 메시가 가장 가까운 적을 바라보도록 Target 갱신)
	DetectingEnemy();

	// 범위 내 적 슬로우 적용
	ApplyRangeDebuff();
}

void ATGDebuffTower::ApplyRangeDebuff()
{
	ATGNavigationManager* NavigationManager = ATGNavigationManager::Get(this);
	if (!NavigationManager) return;

	const FVector Center = GetActorLocation();
	const float DebuffRangeSquared = FMath::Square(DebuffRange);

	// 이번 틱에 범위 내에 있어 슬로우를 적용한 대상 목록
	TArray<TObjectPtr<AActor>> StillSlowed;
	for (AActor* Candidate : NavigationManager->GetTargetables())
	{
		if (!IsValid(Candidate)) continue;
		ITGTargetable* Targetable = Cast<ITGTargetable>(Candidate);
		if (!Targetable || !Targetable->IsTargetable()) continue;
		if (FVector::DistSquared(Center, Candidate->GetActorLocation()) > DebuffRangeSquared) continue;

		Targetable->SetMoveSlowMultiplier(SlowRate);
		StillSlowed.Add(Candidate);
	}

	// 지난 틱엔 슬로우했지만 이번에 범위를 벗어난 대상은 속도 복구
	for (const TObjectPtr<AActor>& Prev : SlowedTargets)
	{
		if (StillSlowed.Contains(Prev)) continue;
		if (IsValid(Prev))
		{
			if (ITGTargetable* Targetable = Cast<ITGTargetable>(Prev.Get()))
			{
				Targetable->SetMoveSlowMultiplier(1.f);
			}
		}
	}

	SlowedTargets = MoveTemp(StillSlowed);
}

void ATGDebuffTower::StopDebuff()
{
	// 타워 파괴 시 슬로우된 모든 대상 속도 복구
	for (const TObjectPtr<AActor>& Prev : SlowedTargets)
	{
		if (IsValid(Prev))
		{
			if (ITGTargetable* Targetable = Cast<ITGTargetable>(Prev.Get()))
			{
				Targetable->SetMoveSlowMultiplier(1.f);
			}
		}
	}
	SlowedTargets.Empty();
}

void ATGDebuffTower::Upgrade()
{
	// 슬로우 비율 증가 (더 느리게) 후 부모 호출로 UpgradeLevel++
	SlowRate = FMath::Max(0.1f, SlowRate - 0.1f);
	Super::Upgrade();
}

void ATGDebuffTower::DetectingEnemy()
{
	Target = nullptr;

	//	NavigationManager의 생존 적 목록에서 사거리 내 가장 가까운 적을 찾습니다.
	//	(매 틱 GetAllActorsOfClass 전체 스캔 제거)
	ATGNavigationManager* NavigationManager = ATGNavigationManager::Get(this);
	if (!NavigationManager) return;

	const FVector MyLocation = GetActorLocation();
	float MinDistanceSquared = FMath::Square(DebuffRange);
	for (AActor* Candidate : NavigationManager->GetTargetables())
	{
		if (!IsValid(Candidate)) continue;
		const ITGTargetable* Targetable = Cast<ITGTargetable>(Candidate);
		if (!Targetable || !Targetable->IsTargetable()) continue;

		const float DistanceSquared = FVector::DistSquared(MyLocation, Candidate->GetActorLocation());
		if (DistanceSquared < MinDistanceSquared)
		{
			MinDistanceSquared = DistanceSquared;
			Target = Candidate;
		}
	}
}

