// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TGSwarmSpawner.generated.h"

class ATGSwarmEnemy;
class USceneComponent;

// 경량 적(ATGSwarmEnemy)을 일정 간격으로 대량 스폰하는 단순 스포너.
// 레벨에 배치 후 EnemyClass/Count 등을 지정해 사용한다.
UCLASS()
class TOWERGAME_API ATGSwarmSpawner : public AActor
{
	GENERATED_BODY()

public:
	ATGSwarmSpawner();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// 스폰할 경량 적 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swarm Spawn")
	TSubclassOf<ATGSwarmEnemy> EnemyClass;

	// 총 스폰 수 (0 이하면 무한)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swarm Spawn")
	int32 SpawnCount = 100;

	// 스폰 간격 (초)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swarm Spawn", meta = (ClampMin = "0.01"))
	float SpawnInterval = 0.2f;

	// 1회 스폰당 마리 수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swarm Spawn", meta = (ClampMin = "1"))
	int32 SpawnPerTick = 1;

	// 스폰 반경 (스포너 위치 기준 랜덤 분산)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swarm Spawn", meta = (ClampMin = "0.0"))
	float SpawnRadius = 500.f;

	// BeginPlay 시 자동 시작
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swarm Spawn")
	bool bAutoStart = true;

	UFUNCTION(BlueprintCallable, Category = "Swarm Spawn")
	void StartSpawning();

	UFUNCTION(BlueprintCallable, Category = "Swarm Spawn")
	void StopSpawning();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;

private:
	void SpawnTick();
	void SpawnOne();

	FTimerHandle SpawnTimerHandle;
	int32 SpawnedCount = 0;
};
