// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TGMountedTower.h"
#include "Enemies/TGEnemyBase.h"
#include "DebuffTower.generated.h"

UCLASS()
class TOWERGAME_API ATGDebuffTower : public ATGMountedTower
{
	GENERATED_BODY()

public:
	ATGDebuffTower();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// 슬로우 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower Settings")
	float DebuffRange = 500.f;

	// 슬로우 비율 (0.5 = 이동속도 50% 감소)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower Settings")
	float SlowRate = 0.5f;

	// 슬로우된 적과 원래 속도를 저장하는 맵 — 범위 벗어나면 복구에 사용
	UPROPERTY()
	TMap<ATGEnemyBase*, float> SlowedEnemies;

public:
	// 슬로우 중지 및 모든 적 속도 복구 (타워 파괴 시 호출)
	UFUNCTION(BlueprintCallable, Category = "Tower Logic")
	void StopDebuff();

	// 범위 내 적을 스캔하고 슬로우를 적용하는 핵심 로직
	void ApplyRangeDebuff();

	// 업그레이드 (슬로우 비율 증가 — 더 느리게)
	virtual void Upgrade() override;

	// 현재 슬로우 범위 반환
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tower Settings")
	float GetDebuffRange() const { return DebuffRange; }

private:
	//	범위 내 가장 가까운 적 탐지
	void DetectingEnemy();
};
