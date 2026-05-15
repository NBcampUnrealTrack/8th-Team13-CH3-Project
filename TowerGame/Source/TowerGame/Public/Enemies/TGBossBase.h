// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TGBossBase.generated.h"

class ATGPlayer;
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

	// 코드 책임 상 Player는 Phase보다 BossBase가 들고 있는게 바람직함
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Boss|Target")
	ATGPlayer* GetPlayer() const;

protected:
	// PhaseClasses[Index]를 현재 State 객체로 생성하고 전환한다.
	void ChangePhase(int32 NewPhaseIndex);
	// 현재 HP 비율이 PhaseHPRatio 기준 이하인지 확인한다.
	void CheckPhaseTransition();

	// Boss 본체 HP만 감소시킨다. 부위 파괴는 이후 단계에서 분리한다.
	void ApplyBossDamage(float DamageAmount);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Stat")
	float MaxHP;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Stat")
	float CurrentHP;

	// 파생 Boss가 사용할 Phase 클래스를 순서대로 등록한다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Phase")
	TArray<TSubclassOf<UTGBossPhaseBase>> PhaseClasses;

	// 현재 실행 중인 State 객체.
	UPROPERTY()
	TObjectPtr<UTGBossPhaseBase> CurrentPhase;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Phase")
	int32 CurrentPhaseIndex;

	// Index N은 Phase N에서 Phase N+1로 넘어가는 HP 비율이다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Phase")
	TArray<float> PhaseHPRatio;

	// 싱글 플레이 기준 Boss가 공격 대상으로 사용할 Player.
	UPROPERTY()
	TObjectPtr<ATGPlayer> TargetPlayer;
};
