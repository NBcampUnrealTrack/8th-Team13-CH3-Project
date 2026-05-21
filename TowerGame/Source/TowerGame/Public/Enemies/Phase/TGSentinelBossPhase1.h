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

};
