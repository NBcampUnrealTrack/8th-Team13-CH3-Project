// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Grid/TGGridBase.h"

#include "Core/Grid/TGSingleGrid.h"
#include "Enemies/TGEnemySpawner.h"
#include "Enemies/TGNavigationManager.h"
#include "Enemies/TGCoreBase.h"


ATGGridBase::ATGGridBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

ATGSingleGrid* ATGGridBase::GetSingleGridFromPosition( FVector Position)
{
	FVector RelativePosition = Position - GetActorLocation();
	int X = RelativePosition.X / GridSize;
	int Y = RelativePosition.Y / GridSize;
	if (X < 0 || X >= GridX || Y < 0 || Y >= GridY)	return nullptr;
	return GridActors[Y][X];
}

ATGSingleGrid* ATGGridBase::GetSingleGridFromPoint(const FIntPoint Point)
{
	if (Point.X < 0 || Point.X >= GridX)	return nullptr;
	if (Point.Y < 0 || Point.Y >= GridY)	return nullptr;
	return GridActors[Point.Y][Point.X];
}

FIntPoint ATGGridBase::GetPointFromPosition(FVector Position) const
{
	FVector RelativePosition = Position - GetActorLocation();
	int X = RelativePosition.X / GridSize;
	int Y = RelativePosition.Y / GridSize;
	if (X < 0) X = 0;
	else if (X >= GridX) X = GridX - 1;
	if (Y < 0) Y = 0;
	else if (Y >= GridY) Y = GridY - 1;
	return FIntPoint(X, Y);
}

bool ATGGridBase::CanBePlacedBuilding(FIntPoint Point)
{
	//	1. 진입점을 막으면 안됨
	//	2. 진출점을 막으면 안됨
	if (EntryPoints.Contains(Point))
	{
		return false;
	}
	if (ExitPoints.Contains(Point))
	{
		return false;
	}

	bBuildingPlaced[Point.Y][Point.X] = true;

	//	3. 경로가 막혀있으면 안됨
	if (!PathFinding())
	{
		bBuildingPlaced[Point.Y][Point.X] = false;
		return false;
	}

	return true;
}

void ATGGridBase::PlacingBuilding(FIntPoint Point)
{
	bBuildingPlaced[Point.Y][Point.X] = true;

	ATGNavigationManager::Get(this)->NotifyBuildingPlaced();
}

void ATGGridBase::RemoveBuilding(FIntPoint Point)
{
	bBuildingPlaced[Point.Y][Point.X] = false;
	if (Point.Y == 0)
	{
		EntryPoints.Add(Point);
	}
	if (Point.Y == GridY - 1)
	{
		ExitPoints.Add(Point);
	}

	ATGNavigationManager::Get(this)->NotifyBuildingPlaced();
}

void ATGGridBase::AddEntryPoint(FIntPoint Point)
{
	EntryPoints.Add(Point);
	//	스포너를 생성합니다
	if (!ensureMsgf(EnemySpawnerClass, TEXT("ATGGridBase : EnemySpawnClass가 설정되지 않았습니다!"))) return;
	ATGEnemySpawner* EnemySpawner = GetWorld()->SpawnActor<ATGEnemySpawner>(EnemySpawnerClass);
	if (!IsValid(EnemySpawner))	return;

	//	스포너의 월드 위치를 조정합니다
	FVector SpawnPos = ConvertIndexToVector(Point);
	EnemySpawner->SetActorLocation(SpawnPos);
}

void ATGGridBase::RemoveEntryPoint(FIntPoint Point)
{
	EntryPoints.Remove(Point);
	TObjectPtr<ATGEnemySpawner> Spawner;
	//	진입점 목록에서 스포너를 찾고 제거합니다
	if (EnemySpawners.RemoveAndCopyValue(Point, Spawner))
	{
		Spawner->Destroy();
	}
}

void ATGGridBase::AddExitPoint(FIntPoint Point)
{
	ExitPoints.Add(Point);
	if (!ensureMsgf(CoreClass, TEXT("ATGGridBase : CoreClass가 설정되지 않았습니다!"))) return;
	ATGCoreBase* Core = GetWorld()->SpawnActor<ATGCoreBase>(CoreClass);
	if (!IsValid(Core))	return;

	FVector CorePos = ConvertIndexToVector(Point);
	Core->SetActorLocation(CorePos);
}

void ATGGridBase::RemoveExitPoint(FIntPoint Point)
{
	ExitPoints.Remove(Point);
	TObjectPtr<ATGCoreBase> Core;
	if (Cores.RemoveAndCopyValue(Point, Core))
	{
		Core->Destroy();
	}
}

FVector ATGGridBase::ConvertIndexToVector(FIntPoint Point)
{
	const FVector OriginPos = GetActorLocation();
	return FVector(
		OriginPos.X + (GridSize * Point.X) + (GridSize / 2.0f),
		OriginPos.Y + (GridSize * Point.Y) + (GridSize / 2.0f),
		OriginPos.Z
	);
}

void ATGGridBase::BeginPlay()
{
	Super::BeginPlay();

	const FVector OriginPos = GetActorLocation();

	GridActors.SetNum(GridY);
	bBuildingPlaced.SetNum(GridY);
	for (int32 i = 0; i < GridY; i++)
	{
		GridActors[i].Reserve(GridX);
		bBuildingPlaced[i].Reserve(GridX);
		for (int32 j = 0; j < GridX; j++)
		{
			FVector GridPos = OriginPos;
			GridPos.X += (GridSize * j) + (GridSize / 2);
			GridPos.Y += (GridSize * i) + (GridSize / 2);
			const FTransform GridTransform(FRotator::ZeroRotator, GridPos);

			ATGSingleGrid* NewGrid = GetWorld()->SpawnActorDeferred<ATGSingleGrid>(SingleGridClass, GridTransform);
			check(NewGrid != nullptr);
			NewGrid->SetParent(this);
			NewGrid->FinishSpawning(GridTransform);
			NewGrid->SetBoxSize(GridSize);
			GridActors[i].Add(NewGrid);
			bBuildingPlaced[i].Add(false);
		}
	}
	//FindSpawnPoints();
	//FindCorePoints();
}

void ATGGridBase::PlacingGrid()
{
	FVector OriginPos = GetActorLocation();

	for (int32 i = 0; i < GridY; i++)
	{
		for (int32 j = 0; j < GridX; j++)
		{
			ATGSingleGrid* SingleGrid = GridActors[i][j];
			FVector GridPos = OriginPos;
			GridPos.X += (GridSize * j) + (GridSize / 2);
			GridPos.Y += (GridSize * i) + (GridSize / 2);
			SingleGrid->SetActorLocation(GridPos);
		}
	}
}

//	월드에 배치된 스폰 포인트를 모두 찾아서 엔트리 포인트로 등록합니다
/*void ATGGridBase::FindSpawnPoints()
{
	for (TActorIterator<ATGEnemySpawner> It(GetWorld()); It; ++It)
	{
		FVector pos = It->GetActorLocation();
		FIntPoint point = GetPointFromPosition(pos);
		AddEntryPoint(point);
	}

	if (EntryPoints.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("ATGGridBase: 월드에서 ATGEnemySpawner를 찾지 못했습니다."));
	}
}*/

//	월드에 배치된 코어를 모두 찾아서 엑싯 포인트로 등록합니다
/*void ATGGridBase::FindCorePoints()
{
	for (TActorIterator<ATGCoreBase> It(GetWorld()); It; ++It)
	{
		FVector pos = It->GetActorLocation();
		FIntPoint point = GetPointFromPosition(pos);
		AddExitPoint(point);
	}

	if (ExitPoints.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("ATGGridBase: 월드에서 ATGCoreBase를 찾지 못했습니다."));
	}
}*/

bool ATGGridBase::PathFinding()
{	//	경로간 비용이 없으므로 BFS로 패스파인딩
	//	모든 진출점 → 역방향 BFS → 도달 가능 셀 집합 구성
	//	이후 모든 진입점이 해당 집합에 포함되는지 확인 (O(GridX * GridY), 1회 탐색으로 해결)

	TArray<TArray<bool>> Reachable;
	Reachable.SetNum(GridY);
	for (int32 i = 0; i < GridY; i++)
	{
		Reachable[i].Init(false, GridX);
	}

	//	모든 열린 진출점을 동시에 출발점으로 추가 (멀티소스 역방향 BFS)
	TArray<FIntPoint> Queue;
	for (const FIntPoint& Exit : ExitPoints)
	{
		Reachable[Exit.Y][Exit.X] = true;
		Queue.Add(Exit);
	}

	//	탐색 방향
	const int32 DX[] = { 1, -1, 0, 0 };
	const int32 DY[] = { 0, 0, 1, -1 };

	for (int32 Head = 0; Head < Queue.Num(); Head++)
	{
		const FIntPoint Current = Queue[Head];

		for (int32 d = 0; d < 4; d++)
		{
			const int32 NX = Current.X + DX[d];
			const int32 NY = Current.Y + DY[d];

			if (NX < 0 || NX >= GridX || NY < 0 || NY >= GridY) continue;
			if (Reachable[NY][NX] || bBuildingPlaced[NY][NX]) continue;

			Reachable[NY][NX] = true;
			Queue.Add(FIntPoint(NX, NY));
		}
	}

	//	모든 진입점이 진출점으로 도달 가능한지 확인
	for (const FIntPoint& Entry : EntryPoints)
	{
		if (!Reachable[Entry.Y][Entry.X])
		{
			return false;
		}
	}

	return true;
}
