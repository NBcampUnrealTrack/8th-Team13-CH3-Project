// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TGWaveManager.generated.h"

class ATGEnemySpawner;
class ATGEnemyBase;

USTRUCT(BlueprintType)
// 몬스터 스폰 수
struct FTGEnemySpawnInfo
{
	GENERATED_BODY()

	FTGEnemySpawnInfo();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	TSubclassOf<ATGEnemyBase> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave", meta = (ClampMin = 0))
	int32 SpawnCount;
};

USTRUCT(BlueprintType)
// 웨이브 몬스터 스폰 정보
struct FTGWaveInfo
{
	GENERATED_BODY()

	FTGWaveInfo();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	TArray<FTGEnemySpawnInfo> EnemySpawnInfos;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave", meta = (ClampMin = "0.01"))
	float SpawnInterval;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave", meta = (ClampMin = "0"))
	float DelayAfterSpawnCompleted;
};

// 웨이브 진행 상태 이벤트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveStarted, int32, WaveIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWaveSpawnCompleted, int32, WaveIndex, bool, bCanStartNextWave);

UCLASS()
class TOWERGAME_API ATGWaveManager : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	TObjectPtr<ATGEnemySpawner> EnemySpawner;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	TArray<FTGWaveInfo> WaveInfos;

private:
	// 스폰 타이머
	FTimerHandle SpawnTimerHandle;
	FTimerHandle NextWaveDelayTimerHandle;

	// 웨이브 인덱스
	int32 NextWaveIndex;
	int32 ActiveWaveIndex;

	// Spawn 인덱스
	bool bIsSpawning;
	int32 CurrentSpawnInfoIndex;
	int32 SpawnedCountInCurrentInfo;

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
	void SetEnemySpawner(ATGEnemySpawner* InEnemySpawner);
	void ClearEnemySpawner(ATGEnemySpawner* InEnemySpawner);

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
	bool MoveToNextSpawnInfo();
	void FinishCurrentWaveSpawn();
	void HandleNextWaveDelayFinished();
	void InitializeSpawnState(int32 WaveIndex);
	bool HasNextWave() const;
};
