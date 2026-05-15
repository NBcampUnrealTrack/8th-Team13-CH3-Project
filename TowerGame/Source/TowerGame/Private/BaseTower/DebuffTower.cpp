// Fill out your copyright notice in the Description page of Project Settings.


#include "TGDebuffTower.h"
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

	// DebuffInterval(초)마다 ApplyRangeDebuff 무한 반복
	StartDebuff();
}

void ATGDebuffTower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//	적 탐지
	DetectingEnemy();
	//	사거리 그려줄 디버그스피어
	DrawDebugSphere(GetWorld(), GetActorLocation(), DebuffRange, 16, FColor::Purple);
}

void ATGDebuffTower::StartDebuff()
{
	// DebuffInterval(초)마다 ApplyRangeDebuff 무한 반복
	GetWorld()->GetTimerManager().SetTimer(
		DebuffTimerHandle,
		this,
		&ATGDebuffTower::ApplyRangeDebuff,
		DebuffInterval,
		true
	);
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

	bool bHasOverlap = UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		Center,
		DebuffRange,
		ObjectTypes,
		nullptr,
		IgnoreActors,
		OutActors
	);

	// 디버그용 구체 (개발 완료 후 삭제 가능)
	DrawDebugSphere(GetWorld(), Center, DebuffRange, 12, FColor::Purple, false, DebuffInterval);

	if (bHasOverlap)
	{
		for (AActor* Actor : OutActors)
		{
			if (ATGEnemyBase* Enemy = Cast<ATGEnemyBase>(Actor))
			{
				// 적 이동속도를 SlowRate 비율만큼 감소
				if (UCharacterMovementComponent* Movement = Enemy->GetCharacterMovement())
				{
					float OriginalSpeed = Movement->MaxWalkSpeed;
					Movement->MaxWalkSpeed = OriginalSpeed * SlowRate;

					// SlowDuration 후 원래 속도로 복구
					FTimerHandle RestoreHandle;
					GetWorld()->GetTimerManager().SetTimer(
						RestoreHandle,
						[Movement, OriginalSpeed]()
						{
							if (Movement)
								Movement->MaxWalkSpeed = OriginalSpeed;
						},
						SlowDuration,
						false
					);
				}
			}
		}
	}
}

void ATGDebuffTower::StopDebuff()
{
	GetWorld()->GetTimerManager().ClearTimer(DebuffTimerHandle);
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

