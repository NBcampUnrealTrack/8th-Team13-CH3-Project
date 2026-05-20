// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseTower/BuffTower.h"
#include "Kismet/GameplayStatics.h"
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

	// 생성 시 플레이어 한 번만 탐색해서 저장
	CachedPlayer = Cast<ATGPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));

	// HealInterval(초)마다 ApplyRangeHeal 무한 반복
	StartHeal();

	SetRangeSphereScale(BuffRange);
}

void ATGBuffTower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
	// 플레이어가 없으면 무시
	if (!IsValid(CachedPlayer)) return;

	// 플레이어까지 거리 계산 후 범위 안에 있으면 힐 적용
	float Distance = FVector::Distance(GetActorLocation(), CachedPlayer->GetActorLocation());
	if (Distance <= BuffRange)
	{
		CachedPlayer->ChangePlayerHP(HealAmount);
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

