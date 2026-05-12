// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TGWaveManager.generated.h"

class ATGEnemySpawner;
class ATGEnemyBase;
class UTGWaveData;

// 웨이브 진행 상태 이벤트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveStarted, int32, WaveIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWaveSpawnCompleted, int32, WaveIndex, bool, bCanStartNextWave);

UCLASS()
class TOWERGAME_API ATGWaveManager : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave", meta = (ClampMin = "0.01"))
	float SpawnInterval;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave", meta = (ClampMin = "0"))
	float DelayBeforeNextWave;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	TArray<TObjectPtr<UTGWaveData>> WaveDataList;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	TArray<TObjectPtr<ATGEnemySpawner>> EnemySpawners;

private:
	// 스폰 타이머
	FTimerHandle SpawnTimerHandle;
	FTimerHandle NextWaveDelayTimerHandle;

	// 웨이브 인덱스
	int32 NextWaveIndex;
	int32 ActiveWaveIndex;

	// Spawn 인덱스
	bool bIsSpawning;
	TArray<TSubclassOf<ATGEnemyBase>> PendingSpawnQueue;
	int32 PendingSpawnIndex;
	int32 NextSpawnerIndex;

public:
	ATGWaveManager();

protected:
	//Unreal LifeCycle
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// 싱글톤 인스턴스 반환
	static  ATGWaveManager* Get(const UObject* WorldContextObject);

private:
	static TWeakObjectPtr<ATGWaveManager> Instance;

public:
	void AddEnemySpawner(ATGEnemySpawner* InEnemySpawner);
	void RemoveEnemySpawner(ATGEnemySpawner* InEnemySpawner);

public:
	// Wave 시작
	UFUNCTION(BlueprintCallable, Category = "Wave")
	bool StartNextWave();

public:
	// Wave Event
	UPROPERTY(BlueprintAssignable, Category = "Wave")
	FOnWaveStarted OnWaveStarted;

	UPROPERTY(BlueprintAssignable, Category = "Wave")
	FOnWaveSpawnCompleted OnWaveSpawnCompleted;

private:
	// Wave 관리
	void SpawnNextEnemy();
	void FinishCurrentWaveSpawn();
	void HandleNextWaveDelayFinished();
	void InitializeSpawnState(int32 WaveIndex);
	bool HasNextWave() const;
};
