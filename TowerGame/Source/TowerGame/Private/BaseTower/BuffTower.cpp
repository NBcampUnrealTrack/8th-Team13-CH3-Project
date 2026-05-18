// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseTower/BuffTower.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Player/TGPlayer.h"

ATGBuffTower::ATGBuffTower()
{
	PrimaryActorTick.bCanEverTick = true;

	//	기본적으로 인터랙션을 끕니다
	SetInteractionEnabled(false);
}

void ATGBuffTower::BeginPlay()
{
	Super::BeginPlay();

	// HealInterval(초)마다 ApplyRangeHeal 무한 반복
	StartHeal();
}

void ATGBuffTower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//	사거리 그려줄 디버그스피어
	//DrawDebugSphere(GetWorld(), GetActorLocation(), BuffRange, 16, FColor::Green);
}

void ATGBuffTower::StartHeal()
{
	// HealInterval(초)마다 ApplyRangeHeal 무한 반복
	GetWorld()->GetTimerManager().SetTimer(
		HealTimerHandle,
		this,
		&ATGBuffTower::ApplyRangeHeal,
		HealInterval,
		true
	);
}

void ATGBuffTower::ApplyRangeHeal()
{
	FVector Center = GetActorLocation();

	// 감지 대상: Pawn 타입 (플레이어)
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	// 자기 자신은 제외
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);

	TArray<AActor*> OutActors;

	bool bHasOverlap = UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		Center,
		BuffRange,
		ObjectTypes,
		nullptr,
		IgnoreActors,
		OutActors
	);

	// 디버그용 구체 (개발 완료 후 삭제 가능)
	//DrawDebugSphere(GetWorld(), Center, BuffRange, 12, FColor::Green, false, HealInterval);

	if (bHasOverlap)
	{
		for (AActor* Actor : OutActors)
		{
			// 범위 내 플레이어에게 체력 회복
			if (ATGPlayer* Player = Cast<ATGPlayer>(Actor))
			{
				Player->ChangePlayerHP(HealAmount);
			}
		}
	}
}

void ATGBuffTower::StopHeal()
{
	GetWorld()->GetTimerManager().ClearTimer(HealTimerHandle);
}

void ATGBuffTower::Upgrade()
{
	// 회복량 증가 후 부모 호출로 UpgradeLevel++
	HealAmount += 5;
	Super::Upgrade();
}

