// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseTower/DebuffTower.h"
#include "Enemies/TGEnemyBase.h"
#include "Enemies/TGNavigationManager.h"
#include "GameFramework/CharacterMovementComponent.h"

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

	// 이번 틱에 범위 내에 있는 적 목록 (생존 적 목록 사용 — 오버랩 쿼리 제거)
	TArray<ATGEnemyBase*> EnemiesInRange;
	for (ATGEnemyBase* Enemy : NavigationManager->GetAliveEnemies())
	{
		if (!IsValid(Enemy) || Enemy->IsDead()) continue;
		if (FVector::DistSquared(Center, Enemy->GetActorLocation()) > DebuffRangeSquared) continue;

		EnemiesInRange.Add(Enemy);

		if (UCharacterMovementComponent* Movement = Enemy->GetCharacterMovement())
		{
			// 기본속도 기준으로 SlowRate 적용
			// 이미 슬로우 중이면 중복 적용 방지
			if (!SlowedEnemies.Contains(Enemy)) {

				// 기본속도 저장 후 슬로우 적용
				SlowedEnemies.Add(Enemy, Movement->MaxWalkSpeed);
				Movement->MaxWalkSpeed *= SlowRate;
			}
		}
	}

	// 범위 벗어난 적 원래 속도로 복구
	for (auto It = SlowedEnemies.CreateIterator(); It; ++It)
	{
		ATGEnemyBase* Enemy = It->Key;
		if (!EnemiesInRange.Contains(Enemy))
		{
			// 범위 밖으로 나간 적 — 원래 속도 복구 (파괴 대기중인 적은 IsValid가 걸러냄)
			if (IsValid(Enemy) && Enemy->GetCharacterMovement())
			{
				Enemy->GetCharacterMovement()->MaxWalkSpeed = It->Value;
			}
			It.RemoveCurrent();
		}
	}
}

void ATGDebuffTower::StopDebuff()
{
	// 타워 파괴 시 슬로우된 모든 적 속도 복구
	for (auto& Pair : SlowedEnemies)
	{
		if (IsValid(Pair.Key) && Pair.Key->GetCharacterMovement())
		{
			Pair.Key->GetCharacterMovement()->MaxWalkSpeed = Pair.Value;
		}
	}
	SlowedEnemies.Empty();
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
	for (ATGEnemyBase* Enemy : NavigationManager->GetAliveEnemies())
	{
		if (!IsValid(Enemy) || Enemy->IsDead()) continue;
		const float DistanceSquared = FVector::DistSquared(MyLocation, Enemy->GetActorLocation());
		if (DistanceSquared < MinDistanceSquared)
		{
			MinDistanceSquared = DistanceSquared;
			Target = Enemy;
		}
	}
}

