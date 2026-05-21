// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/TGPatternBase.h"
#include "TGGroundCylinderPattern.generated.h"

/**
 *
 */
UCLASS()
class TOWERGAME_API UTGGroundCylinderPattern : public UTGPatternBase
{
	GENERATED_BODY()

public:
	UTGGroundCylinderPattern();

	virtual void GetAttackLocation() override;

	virtual void DrawWarning(float WarningDrawTime) override;

protected:
	virtual void CollectDamageTargets(TArray<AActor*>& OutTargets) const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Pattern")
	float CylinderHeight;
};
