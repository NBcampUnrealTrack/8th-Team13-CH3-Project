// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TGEnemySpawner.generated.h"

class ATGNavigationManager;
class ATGEnemyBase;

UCLASS()
class TOWERGAME_API ATGEnemySpawner : public AActor
{
	GENERATED_BODY()

public:
	// Unreal LifeCycle
	ATGEnemySpawner();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	// Enemy의 SpawnOffSet 재 계산
	void UpdateSpawnLocationOffset(TSubclassOf<ATGEnemyBase> EnemyClass);

public:
	// Enemy 생성
	ATGEnemyBase* SpawnEnemy(TSubclassOf<ATGEnemyBase> EnemyClass);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Navigation")
	TObjectPtr<ATGNavigationManager> NavigationManager;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	FVector  SpawnLocationOffset;

private:
	TSubclassOf<ATGEnemyBase> LastSpawnedEnemyClass;
};
