// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/TGEnemyBase.h"
#include "TGAttackEnemyBase.generated.h"

class ATGMountedTower;
class ATGPlayer;
/**
 *
 */
UCLASS()
class TOWERGAME_API ATGAttackEnemyBase : public ATGEnemyBase
{
	GENERATED_BODY()

protected:
	// Player 탐지 관련 변수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|PlayerAttack")
	float PlayerDetectRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|PlayerAttack")
	float PlayerDetectInterval;

	// 공격 정보 관련 변수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|PlayerAttack")
	float AreaAttackRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|PlayerAttack")
	float AreaAttackDelay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|PlayerAttack")
	float AreaAttackDamage;

	// Player에게 부정적인 영향을 주는 값
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|PlayerAttack")
	float KnockbackForce;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|PlayerAttack")
	float DebuffDuration;

	// Player 공격 및 탐지 타이머
	FTimerHandle AreaAttackTimerHandle;
	FTimerHandle PlayerDetectTimerHandle;

public:
	// Unreal LifeCycle
	ATGAttackEnemyBase();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void RequestRepath() override;
	virtual void StartAttack() override;

protected:
	// Player 탐지
	void DetectPlayerInRange();

	// Player 공격
	void StartPlayerAttack(AActor* Player);
	void StopPlayerAttack();
	void AreaAttack();

	// 원형 공격 확장을 위한 함수
	virtual bool CanAreaAttackTarget(AActor* Target) const;
	virtual void ApplyAreaAttackEffect(AActor* Target);

	void ApplyPlayerAttackEffect(ATGPlayer* Player);
	void ApplyTowerAttackEffect(ATGMountedTower* Tower);
};
