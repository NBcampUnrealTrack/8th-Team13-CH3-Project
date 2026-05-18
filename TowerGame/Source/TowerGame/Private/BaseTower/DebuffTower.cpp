// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseTower/DebuffTower.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Enemies/TGEnemyBase.h"
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


}

void ATGDebuffTower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//	적 탐지
	DetectingEnemy();

	//	사거리 그려줄 디버그스피어
	DrawDebugSphere(GetWorld(), GetActorLocation(), DebuffRange, 16, FColor::Purple);

	// 범위 내 적 슬로우 적용
	ApplyRangeDebuff();
}

void ATGDebuffTower::ApplyRangeDebuff()
{
	FVector Center = GetActorLocation();

	// 감지 대상: Pawn 타입 (몬스터)
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	// 자기 자신은 제외
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);

	TArray<AActor*> OutActors;
	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(), Center, DebuffRange,
		ObjectTypes, nullptr, IgnoreActors, OutActors
	);

	// 이번 틱에 범위 내에 있는 적 목록
	TArray<ATGEnemyBase*> EnemiesInRange;
	for (AActor* Actor : OutActors)
	{
		if (ATGEnemyBase* Enemy = Cast<ATGEnemyBase>(Actor))
		{
			EnemiesInRange.Add(Enemy);

			if (UCharacterMovementComponent* Movement = Enemy->GetCharacterMovement())
			{
				// 기본속도 기준으로 SlowRate 적용
				// 이미 슬로우 중이면 중복 적용 방지
				if (!SlowedEnemies.Contains(Enemy))
				{
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
			// 범위 밖으로 나간 적 — 원래 속도 복구
			if (Enemy && Enemy->GetCharacterMovement())
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
		if (Pair.Key && Pair.Key->GetCharacterMovement())
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
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATGEnemyBase::StaticClass(), FoundActors);

	float MinDistance = 500000.f;
	Target = nullptr;
	for (AActor* Actor : FoundActors)
	{
		FVector MyLocation = GetActorLocation();
		FVector TargetLocation = Actor->GetActorLocation();
		float Distance = FVector::Distance(MyLocation, TargetLocation);
		if (Distance > DebuffRange) continue;
		if (Distance < MinDistance)
		{
			MinDistance = Distance;
			Target = Actor;
		}
	}
}

