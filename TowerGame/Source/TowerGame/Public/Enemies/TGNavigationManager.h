// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TGNavigationManager.generated.h"

class ANavigationData;
class ATGCoreBase;
class ATGEnemyBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrentCoreChanged, ATGCoreBase*, CurrentCore);

UCLASS()
class TOWERGAME_API ATGNavigationManager : public AActor
{
	GENERATED_BODY()

public:
	// Unreal LifeCycle
	ATGNavigationManager();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	static ATGNavigationManager* Get(const UObject* WorldContextObject);

	// CoreBase 등록
	void AddCoreActor(ATGCoreBase* InCoreActor);
	void RemoveCoreActor(ATGCoreBase* InCoreActor);

	// 몬스터 등록
	void RegisterEnemy(ATGEnemyBase* Enemy);
	void UnRegisterEnemy(ATGEnemyBase* Enemy);

	// 생존 중인 적 목록 (타워 탐지 등에서 전체 액터 스캔 대신 사용)
	const TArray<TObjectPtr<ATGEnemyBase>>& GetAliveEnemies() const { return AliveEnemies; }

	// 타워 타겟 가능 대상(ITGTargetable 구현체) 등록/조회 — 적 구현 종류와 무관하게 타워가 사용
	void RegisterTargetable(AActor* Target);
	void UnRegisterTargetable(AActor* Target);
	const TArray<TObjectPtr<AActor>>& GetTargetables() const { return Targetables; }

	FVector GetCoreLocation() const;
	ATGCoreBase* GetCurrentCoreActor() const;

	void NotifyBuildingPlaced();

public:
	// 코어 변경 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Navigation")
	FOnCurrentCoreChanged OnCurrentCoreChanged;

private:
	void RepathAllEnemies();

	// NavMesh 갱신 후 호출 될 함수
	UFUNCTION()
	void HandleNavigationGenerationFinished(ANavigationData* NavData);

protected:
	// 목적지
	UPROPERTY(BlueprintReadOnly, Category = "Navigation")
	TArray<TObjectPtr<ATGCoreBase>> CoreActors;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Navigation")
	TObjectPtr<ATGCoreBase> CurrentCoreActor;

private:
	// 경로 재 탐색
	UPROPERTY()
	TArray<TObjectPtr<ATGEnemyBase>> AliveEnemies;

	// 타워 타겟 가능 대상 목록 (ITGTargetable 구현 액터)
	UPROPERTY()
	TArray<TObjectPtr<AActor>> Targetables;

	static TWeakObjectPtr<ATGNavigationManager> Instance;

	bool bPendingRepathAfterNavGeneration = false;
};
