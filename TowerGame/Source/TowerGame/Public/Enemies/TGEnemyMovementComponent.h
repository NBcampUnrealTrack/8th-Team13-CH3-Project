// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "TGEnemyMovementComponent.generated.h"


class ATGNavigationManager;
class ATGEnemyBase;
class ABaseTower;
struct FAIRequestID;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnEnemyAttackRangeReached,
	AActor*,
	TargetActor
);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TOWERGAME_API UTGEnemyMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category= "Enemy|Movement")
	FOnEnemyAttackRangeReached OnAttackRangeReached;

private:
	UPROPERTY()
	TObjectPtr<ATGEnemyBase> OwnerEnemy;

	UPROPERTY()
	TObjectPtr<ATGNavigationManager> NavigationManager;

	UPROPERTY()
	TObjectPtr<AActor> MoveTarget;

	FTimerHandle StructureAttackRangeCheckTimerHandle;

	UPROPERTY(EditAnywhere, Category = "Enemy|Movement")
	int32 GridSize;

	UPROPERTY(EditAnywhere, Category = "Enemy|Movement", meta = (ClampMin = "50.0"))
	float StructureAttackRange;

public:
	// Sets default values for this component's properties
	UTGEnemyMovementComponent();

public:
	void Initialize(ATGEnemyBase* InOwnerEnemy, ATGNavigationManager* InNavigationManager);

	// 경로 재탐색
	void RequestMoveToCore();

	// 공격 범위 탐지 종료
	void StopStructureRangeCheck();

private:
	UFUNCTION()
	void HandleMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	// 공격 범위 기준 경로 이동
	void CheckStructureRange();

	// 공격 범위 X 경로 이동
	void MoveToStructureDirectly();

	bool RecoverToNavMesh();
	bool IsStructureInRange(const AActor* Target) const;

	bool MoveToBlockingBuilding();
	bool TryMoveToAttackRangeOfBuilding(ABaseTower* Building);

};
