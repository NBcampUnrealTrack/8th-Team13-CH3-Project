// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TGSwarmFlockSubsystem.generated.h"

class ATGSwarmEnemy;

// 경량 적들이 서로 겹치지 않도록 분리(separation)력을 계산해 주는 월드 서브시스템.
// 공간 해시를 프레임당 1회만 재구성(GFrameCounter 가드)하므로 적의 틱 순서와 무관하게 동작하고,
// 이웃 탐색이 O(전체)가 아니라 O(주변 셀)로 줄어 수백 마리도 감당한다.
UCLASS()
class TOWERGAME_API UTGSwarmFlockSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// 분리 대상 등록/해제 (경량 적이 BeginPlay/EndPlay에서 호출)
	void RegisterAgent(ATGSwarmEnemy* Agent);
	void UnregisterAgent(ATGSwarmEnemy* Agent);

	// Location 주변 Radius 내 다른 에이전트들로부터 멀어지는 분리 벡터(XY, 거리 가중 합)를 반환.
	// 가까울수록·이웃이 많을수록 크기가 커진다. 호출 측에서 가중치를 곱해 이동 방향에 섞어 쓴다.
	FVector ComputeSeparation(const ATGSwarmEnemy* Self, const FVector& Location, float Radius);

private:
	// 이번 프레임에 아직 안 만들었으면 공간 해시를 재구성
	void RebuildIfNeeded(float CellSize);
	static FIntVector CellCoord(const FVector& Loc, float CellSize);

	UPROPERTY()
	TArray<TObjectPtr<ATGSwarmEnemy>> Agents;

	// 프레임당 1회 재구성되는 공간 해시 (셀 좌표 → Agents 인덱스 목록)
	TArray<FVector> CachedLocations;	// Agents와 동일 인덱스
	TMap<FIntVector, TArray<int32>> Grid;
	uint64 LastRebuildFrame = 0;
	float BuiltCellSize = 0.f;
};
