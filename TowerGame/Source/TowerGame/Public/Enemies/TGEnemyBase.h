// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Navigation/PathFollowingComponent.h"
#include "TGEnemyBase.generated.h"

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

	// 경로
	void RequestRepath();
	void SetNavigationManager(ATGNavigationManager* InNavigationManager);

	// Enemy 공격
	void StartCoreAttack();
	void StopAttack();
	void AttackCore();

protected:
	// Enemy 공격 관련 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Attack")
	float AttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Attack")
	float AttackInterVal;

	FTimerHandle AttackTimerHandle;

protected:
	UFUNCTION()
	void HandleMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

};
