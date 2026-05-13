// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Navigation/PathFollowingComponent.h"
#include "TGEnemyBase.generated.h"

class ABaseTower;
struct FAIRequestID;
class ATGNavigationManager;

UCLASS()
class TOWERGAME_API ATGEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Unreal Life Cycle
	ATGEnemyBase();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	TObjectPtr<ATGNavigationManager> NavigationManager;

public:
	void InitializeEnemy(ATGNavigationManager* InNavigationManager);
	FVector GetNavigationHeightOffset() const;

	// 경로
	virtual void RequestRepath();
	void SetNavigationManager(ATGNavigationManager* InNavigationManager);

	// Enemy 공격
	void StartAttack();
	void AttackTarget();

	void StopAttack();

	virtual float TakeDamage(
		float DamageAmount,
		const FDamageEvent& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser
	) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float HP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Drop")
	int32 EnergyDropAmount = 30;

protected:
	// Enemy 공격 관련 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Attack")
	float AttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Attack")
	float AttackInterVal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Attack", meta = (ClampMin = "50.0"))
	float AttackRange;

	// FlyingEnemy 위치 보정을 위한 FVector
	UPROPERTY(VisibleAnywhere, Category = "Enemy|Spawn")
	FVector NavigationHeightOffset;

	UPROPERTY()
	TObjectPtr<AActor> CurrentAttackTarget;

	FTimerHandle AttackTimerHandle;

private:
	int32 GridSize;

protected:
	UFUNCTION()
	void HandleMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	bool TryRecoverToNearestNavMesh();

	bool IsTargetInAttackRange(const AActor* Target) const;

private:
	// 가까운 건물 반환 추후 우선도 고려
	bool MoveToBlockingBuilding();
	bool TryMoveToAttackRangeOfBuilding(ABaseTower* Building);
};
