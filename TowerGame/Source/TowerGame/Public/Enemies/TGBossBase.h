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
	virtual float TakeDamage(
		float DamageAmount,
		const FDamageEvent& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser
	) override;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Boss|Stat")
	float GetCurrentHP() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Boss|Stat")
	float GetMaxHP() const;

protected:
	// 페이즈 전환
	void ChangePhase(int32 NewPhaseIndex);
	void CheckPhaseTransition();

	// 데미지 적용
	void ApplyBossDamage(float DamageAmount);

protected:
	// HP
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Stat")
	float MaxHP;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Stat")
	float CurrentHP;

	// 사용할 Phase
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Phase")
	TArray<TSubclassOf<UTGBossPhaseBase>> PhaseClasses;

	UPROPERTY()
	TObjectPtr<UTGBossPhaseBase> CurrentPhase;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Phase")
	int32 CurrentPhaseIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Phase")
	TArray<float> PhaseHPRatio;
};
