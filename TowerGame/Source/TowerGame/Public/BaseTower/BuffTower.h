// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TGMountedTower.h"
#include "TGBuffTower.generated.h"

UCLASS()
class TOWERGAME_API ATGBuffTower : public ATGMountedTower
{
	GENERATED_BODY()

public:
	ATGBuffTower();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// 회복 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower Settings")
	float BuffRange = 500.f;

	// 회복량
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower Settings")
	int32 HealAmount = 10;

	// 회복 간격 (초)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower Settings")
	float HealInterval = 3.0f;

	// 회복 반복 타이머 핸들
	FTimerHandle HealTimerHandle;

public:
	// 회복 시작 (타워 설치 완료 시 호출)
	UFUNCTION(BlueprintCallable, Category = "Tower Logic")
	void StartHeal();

	// 회복 중지 (타워 파괴 시 호출)
	UFUNCTION(BlueprintCallable, Category = "Tower Logic")
	void StopHeal();

	// 범위 내 플레이어를 스캔하고 체력을 회복하는 핵심 로직
	void ApplyRangeHeal();

	// 업그레이드 (회복량 증가)
	virtual void Upgrade() override;

	// 현재 회복 범위 반환
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tower Settings")
	float GetBuffRange() const { return BuffRange; }
};
