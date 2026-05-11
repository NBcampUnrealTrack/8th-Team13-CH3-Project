// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "TGGamePlayState.generated.h"

/**
 *
 */
UCLASS()
class TOWERGAME_API ATGGamePlayState : public AGameState
{
	GENERATED_BODY()

public:


	void UpdateHUD();
};
