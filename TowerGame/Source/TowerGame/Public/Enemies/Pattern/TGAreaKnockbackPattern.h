// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/TGPatternBase.h"
#include "TGAreaKnockbackPattern.generated.h"

UCLASS()
class TOWERGAME_API UTGAreaKnockbackPattern : public UTGPatternBase
{
	GENERATED_BODY()

public:
	UTGAreaKnockbackPattern();

	virtual void StartPattern(float WarningDrawTime) override;
	virtual void StopPattern() override;
	virtual void ExecuteAttack() override;

protected:
	virtual void GetAttackLocation() override;
	virtual void DrawWarning(float WarningDrawTime) override;
	virtual void CollectDamageTargets(TArray<AActor*>& OutTargets) const override;

private:
	void StartActiveAttack();
	void ApplyKnockbackToTargets(const TArray<AActor*>& Targets) const;

private:
	UPROPERTY()
	float ActiveDuration;

	UPROPERTY()
	float HitCheckInterval;

	UPROPERTY()
	float KnockbackStrength;

	UPROPERTY()
	float KnockbackUpStrength;

	FTimerHandle ActiveAttackTimerHandle;
	FTimerHandle ActiveAttackEndTimerHandle;
};
