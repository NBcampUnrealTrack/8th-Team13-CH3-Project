// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/TGBossPhaseBase.h"
#include "TGSentinelBossPhase2.generated.h"

/**
 *
 */
UCLASS()
class TOWERGAME_API UTGSentinelBossPhase2 : public UTGBossPhaseBase
{
	GENERATED_BODY()

public:
	UTGSentinelBossPhase2();

	virtual void EnterPhase() override;
	virtual void ExitPhase() override;
};
