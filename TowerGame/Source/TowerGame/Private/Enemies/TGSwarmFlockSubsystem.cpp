// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemies/TGSwarmFlockSubsystem.h"
#include "Enemies/TGSwarmEnemy.h"
#include "CoreGlobals.h"	// GFrameCounter

void UTGSwarmFlockSubsystem::RegisterAgent(ATGSwarmEnemy* Agent)
{
	if (Agent)
	{
		Agents.AddUnique(Agent);
	}
}

void UTGSwarmFlockSubsystem::UnregisterAgent(ATGSwarmEnemy* Agent)
{
	if (Agent)
	{
		Agents.RemoveSwap(Agent);
	}
}

FIntVector UTGSwarmFlockSubsystem::CellCoord(const FVector& Loc, float CellSize)
{
	const float Inv = (CellSize > 0.f) ? (1.f / CellSize) : 0.f;
	return FIntVector(FMath::FloorToInt(Loc.X * Inv), FMath::FloorToInt(Loc.Y * Inv), 0);
}

void UTGSwarmFlockSubsystem::RebuildIfNeeded(float CellSize)
{
	// 이번 프레임에 이미 재구성했으면 스킵 (어떤 적이 먼저 호출하든 프레임당 1회)
	if (GFrameCounter == LastRebuildFrame && BuiltCellSize > 0.f)
	{
		return;
	}

	LastRebuildFrame = GFrameCounter;
	BuiltCellSize = (CellSize > 0.f) ? CellSize : 200.f;

	// 파괴/무효 에이전트 정리
	for (int32 i = Agents.Num() - 1; i >= 0; --i)
	{
		if (!IsValid(Agents[i]))
		{
			Agents.RemoveAtSwap(i);
		}
	}

	// 위치 캐시 + 해시 구성
	Grid.Reset();
	CachedLocations.Reset();
	CachedLocations.Reserve(Agents.Num());

	for (int32 i = 0; i < Agents.Num(); ++i)
	{
		const FVector Loc = Agents[i]->GetActorLocation();
		CachedLocations.Add(Loc);
		Grid.FindOrAdd(CellCoord(Loc, BuiltCellSize)).Add(i);
	}
}

FVector UTGSwarmFlockSubsystem::ComputeSeparation(const ATGSwarmEnemy* Self, const FVector& Location, float Radius)
{
	if (Radius <= 0.f)
	{
		return FVector::ZeroVector;
	}

	RebuildIfNeeded(Radius);

	if (Agents.Num() <= 1)
	{
		return FVector::ZeroVector;
	}

	const float CellSize = BuiltCellSize;
	const int32 Range = FMath::Max(1, FMath::CeilToInt(Radius / CellSize));
	const FIntVector Base = CellCoord(Location, CellSize);
	const float RadiusSq = Radius * Radius;

	FVector Push = FVector::ZeroVector;

	for (int32 dx = -Range; dx <= Range; ++dx)
	{
		for (int32 dy = -Range; dy <= Range; ++dy)
		{
			const TArray<int32>* Bucket = Grid.Find(FIntVector(Base.X + dx, Base.Y + dy, 0));
			if (!Bucket) continue;

			for (int32 Idx : *Bucket)
			{
				if (!CachedLocations.IsValidIndex(Idx)) continue;
				if (Agents[Idx] == Self) continue;

				FVector Diff = Location - CachedLocations[Idx];
				Diff.Z = 0.f;
				const float DistSq = Diff.SizeSquared();
				if (DistSq >= RadiusSq || DistSq <= KINDA_SMALL_NUMBER) continue;

				const float Dist = FMath::Sqrt(DistSq);
				// 가까울수록 강하게 반발 (1 - dist/Radius)
				Push += (Diff / Dist) * (1.f - Dist / Radius);
			}
		}
	}

	return Push;
}
