// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/TGPatternBase.h"
#include "TGPlayerSequenceSpherePattern.generated.h"

/**
 *
 */
UCLASS()
class TOWERGAME_API UTGPlayerSequenceSpherePattern : public UTGPatternBase
{
	GENERATED_BODY()

public:
	UTGPlayerSequenceSpherePattern();

	virtual void StartPattern(float WarningDrawTime) override;
	virtual void StopPattern() override;

protected:
	virtual void CollectDamageTargets(TArray<AActor*>& OutTargets) const override;

private:
	// 경고 범위 및 공격
	void SpawnWarningAtPlayerLocation();
	void StopWarningSpawn();
	void ExecuteAttackAtLocation(FVector InAttackLocation);

private:
	// 경고 위치들 생성 시간
	UPROPERTY()
	float WarningSpawnDuration;

	// Player 위치를 찍는 간격
	UPROPERTY()
	float WarningSpawnInterval;

	// 경고가 사라진 뒤 공격되기까지의 지연 시간
	float AttackDelayAfterWarning;

	TArray<FVector> WarningLocations;

	FTimerHandle WarningSpawnTimerHandle;
	FTimerHandle WarningSpawnStopTimerHandle;
	TArray<FTimerHandle> SequenceAttackTimerHandles;
};
