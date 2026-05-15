// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/TGBossPhaseBase.h"
#include "TGSentinelBossPhase1.generated.h"

/**
 *
 */
UCLASS()
class TOWERGAME_API UTGSentinelBossPhase1 : public UTGBossPhaseBase
{
	GENERATED_BODY()

public:
	UTGSentinelBossPhase1();

	virtual void EnterPhase() override;
	virtual void ExitPhase() override;
	virtual void ExecutePattern() override;

protected:
	void ExecuteDelayedAttack();

protected:
	// Player 위치에 표시할 원형 경고 범위의 반지름.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Pattern")
	float WarningRadius;

	// 다음 패턴 호출 전까지 경고 범위가 유지되는 시간.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Pattern")
	float WarningDrawTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Pattern")
	float AttackDelay;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Pattern")
	float AttackDamage;

	FVector PendingAttackLocation;
};
