// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/TGPatternBase.h"
#include "TGRandomGroundCylinderPattern.generated.h"

UCLASS()
class TOWERGAME_API UTGRandomGroundCylinderPattern : public UTGPatternBase
{
	GENERATED_BODY()

public:
	UTGRandomGroundCylinderPattern();

	virtual void StartPattern(float WarningDrawTime) override;
	virtual void StopPattern() override;

private:
	void SpawnWarnings(float WarningDrawTime);
	void ExecuteAttackLocation(FVector InAttackLocation);
	void CollectCylinderTargets(TArray<AActor*>& OutTargets, const FVector& InAttackLocation) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Pattern")
	float CylinderHeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Pattern")
	float RandomAreaRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Pattern")
	int32 AttackCount;

	TArray<FTimerHandle> AttackTimerHandles;

};
