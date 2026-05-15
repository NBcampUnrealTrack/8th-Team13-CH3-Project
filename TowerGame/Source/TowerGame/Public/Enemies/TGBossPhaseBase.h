// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TGBossPhaseBase.generated.h"

class ATGBossBase;

UCLASS()
class TOWERGAME_API UTGBossPhaseBase : public UObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(ATGBossBase* InOwnerBoss);

	// Phase 관리
	virtual void EnterPhase();
	virtual void ExitPhase();

	// 세부 로직은 각 Phase별 구성
	virtual void ExecutePattern();

protected:
	UPROPERTY()
	TObjectPtr<ATGBossBase> OwnerBoss;
};
