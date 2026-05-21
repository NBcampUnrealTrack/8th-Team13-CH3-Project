// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/TGPatternBase.h"
#include "TGSphereAreaPattern.generated.h"

/**
 *
 */
UCLASS()
class TOWERGAME_API UTGSphereAreaPattern : public UTGPatternBase
{
	GENERATED_BODY()

public:
	UTGSphereAreaPattern();

	virtual void GetAttackLocation() override;
	virtual void DrawWarning(float WarningDrawTime) override;

protected:
	virtual void CollectDamageTargets(TArray<AActor*>& OutTargets) const override;

};
