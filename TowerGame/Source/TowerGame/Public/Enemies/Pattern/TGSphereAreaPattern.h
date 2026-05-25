// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/TGPatternBase.h"
#include "TGSphereAreaPattern.generated.h"

class ATGMissile;
/**
 *
 */
UCLASS()
class TOWERGAME_API UTGSphereAreaPattern : public UTGPatternBase
{
	GENERATED_BODY()

public:
	UTGSphereAreaPattern();

	virtual void StartPattern(float WarningDrawTime) override;
	virtual void StopPattern() override;

	virtual void GetAttackLocation() override;

protected:
	virtual void CollectDamageTargets(TArray<AActor*>& OutTargets) const override;

private:
	UFUNCTION()
	void HandleMissileHit(ATGMissile* Missile, const FHitResult& HitResult);

	UFUNCTION()
	void HandleMissileExpired(ATGMissile* Missile);

	void LaunchMissileToWarningLocation();

private:
	UPROPERTY()
	TObjectPtr<AActor> ActiveWarningActor;

	UPROPERTY()
	TObjectPtr<ATGMissile> ActiveMissile;

	FTimerHandle MissileLaunchTimerHandle;

};
