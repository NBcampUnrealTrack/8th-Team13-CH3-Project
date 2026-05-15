// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/TGWaveData.h"

#include "Enemies/TGEnemyBase.h"

FTGEnemySpawnGroup::FTGEnemySpawnGroup() : EnemyClass(nullptr), SpawnCount(0)
{
}

bool FTGEnemySpawnGroup::IsValid() const
{
	return !EnemyClass.IsNull() && SpawnCount > 0;
}
