// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TGBossBase.generated.h"

class UTGBossPhaseBase;

UCLASS()
class TOWERGAME_API ATGBossBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATGBossBase();

protected:
	// Unreal LifeCycle
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Boss|Stat")
	float GetCurrentHP() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Boss|Stat")
	float GetMaxHP() const;

protected:
	// HP
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Stat")
	float MaxHP;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Stat")
	float CurrentHP;

	// BP / Editor에서 지정하는 Phase
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss|Phase")
	TArray<TSubclassOf<UTGBossPhaseBase>> PhaseClasses;

	// PhaseClasses을 기반으로 Runtime에 생성된 Instance 목록
	UPROPERTY()
	TArray<TObjectPtr<UTGBossPhaseBase>> Phases;

	UPROPERTY()
	TObjectPtr<UTGBossPhaseBase> CurrentPhase;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Phase")
	int32 CurrentPhaseIndex;
};
