// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TGWaveData.generated.h"

class ATGEnemyBase;

USTRUCT(BlueprintType)
struct FTGEnemySpawnGroup
{
	GENERATED_BODY()

	FTGEnemySpawnGroup();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	TSoftClassPtr<ATGEnemyBase> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave", meta = (ClampMin = 0))
	int32 SpawnCount;

	bool IsValid() const;
};

UCLASS()
class TOWERGAME_API UTGWaveData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	TArray<FTGEnemySpawnGroup> SpawnGroups;
};
