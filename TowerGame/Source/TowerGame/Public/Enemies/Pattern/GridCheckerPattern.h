// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/TGPatternBase.h"
#include "GridCheckerPattern.generated.h"

class ATGGridBase;

UCLASS()
class TOWERGAME_API UGridCheckerPattern : public UTGPatternBase
{
	GENERATED_BODY()

public:
	UGridCheckerPattern();

	virtual void StartPattern(float WarningDrawTime) override;
	virtual void StopPattern() override;

private:
	// 경고 범위 생성(데칼)
	void QueueGridWarnings(ATGGridBase* GridBase, float WarningDrawTime);
	void SpawnWarningAtGridPoint(ATGGridBase* GridBase, FIntPoint GridPoint, float WarningDrawTime, bool bFinishPattern);

	// 범위 내 Actor 공격
	void ExecuteAttackAtGridPoint(ATGGridBase* GridBase, FIntPoint GridPoint, const FVector& InAttackLocation);
	void CollectCylinderTargets(TArray<AActor*>& OutTargets, const FVector& InAttackLocation) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Pattern")
	float CylinderHeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Pattern")
	float GridWarningInterval;

private:
	TArray<FTimerHandle> WarningTimerHandles;
	TArray<FTimerHandle> AttackTimerHandles;
};
