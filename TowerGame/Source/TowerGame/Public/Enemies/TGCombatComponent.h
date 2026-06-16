// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TGCombatComponent.generated.h"

// HP/데미지/사망/경험치 드롭을 담당하는 공용 전투 컴포넌트.
// 경량 적(ATGSwarmEnemy)에서 사용하며, 추후 보스/일반 적(ACharacter)도 채택할 수 있도록 분리해 둔다.

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCombatHpChanged, float, CurrentHP, float, MaxHP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCombatDied, AActor*, DeadActor);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TOWERGAME_API UTGCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTGCombatComponent();

protected:
	virtual void BeginPlay() override;

public:
	// 최대 체력
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float MaxHP = 10.f;

	// 처치 시 지급 경험치
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 ExpDropAmount = 10;

	// HP 변동 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Combat")
	FOnCombatHpChanged OnHpChanged;

	// 사망 이벤트 (소유 액터 전달) — 소유자가 구독해 제거/이펙트 처리
	UPROPERTY(BlueprintAssignable, Category = "Combat")
	FOnCombatDied OnDied;

	// 데미지 적용. 실제 적용량 반환. 사망 처리/경험치 지급까지 담당.
	UFUNCTION(BlueprintCallable, Category = "Combat")
	float ApplyDamage(float Amount);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Combat")
	float GetCurrentHP() const { return CurrentHP; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Combat")
	float GetMaxHP() const { return MaxHP; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Combat")
	bool IsDead() const { return bDead; }

private:
	float CurrentHP = 0.f;
	bool bDead = false;
};
