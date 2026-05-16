// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TGActorBase.h"
#include "TGGridBase.generated.h"

class ABaseTower;
class ATGCoreBase;
class ATGEnemySpawner;
class ATGSingleGrid;

UCLASS()
class TOWERGAME_API ATGGridBase : public ATGActorBase
{
	GENERATED_BODY()
	//	월드에 배치되는 그리드 관리 클래스입니다.
public:
	ATGGridBase();

	//	월드 좌표에서 그리드 가져오기
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="TowerGame|Grid")
	ATGSingleGrid* GetSingleGridFromPosition(FVector Position);
	//	2차원 배열 인덱스에서 그리드 가져오기
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="TowerGame|Grid")
	ATGSingleGrid* GetSingleGridFromPoint(const FIntPoint Point);

	//	월드 좌표에서 2차원 배열 인덱스 가져오기
	//	위치를 벗어날 경우에도 안전한 인덱스를 반환합니다.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="TowerGame|Grid")
	FIntPoint GetPointFromPosition(FVector Position) const;

	//	건설 가능한 위치인지 확인
	UFUNCTION(BlueprintCallable, Category="TowerGame|Grid")
	bool CanBePlacedBuilding(FIntPoint Point);

	//	실제로 건설이 가능한지 검사하지 않습니다.
	//	실제로 건설이 가능한 위치인지 먼저 확인 한 후 호출해주세요.
	UFUNCTION(BlueprintCallable, Category="TowerGame|Grid")
	void PlacingBuilding(FIntPoint Point);

	UFUNCTION(BlueprintCallable, Category="TowerGame|Grid")
	void RemoveBuilding(FIntPoint Point);

	//	진입점과 진출점 관리 함수
	//	스포너와 코어를 직접 생성하고 삭제합니다.
	//	게임 시작 전, 최소 한개의 시작점과 도착점을 생성하도록 하세요.
	UFUNCTION(BlueprintCallable, Category="TowerGame|Grid")
	void AddEntryPoint(FIntPoint Point);
	UFUNCTION(BlueprintCallable, Category="TowerGame|Grid")
	void RemoveEntryPoint(FIntPoint Point);
	UFUNCTION(BlueprintCallable, Category="TowerGame|Grid")
	void AddExitPoint(FIntPoint Point);
	UFUNCTION(BlueprintCallable, Category="TowerGame|Grid")
	void RemoveExitPoint(FIntPoint Point);

	//	인덱스에서 월드 위치를 반환합니다
	UFUNCTION(BlueprintCallable, Category="TowerGame|Grid")
	FVector ConvertIndexToVector(FIntPoint Point);

	TSubclassOf<ABaseTower> GetTowerClassOf()	{return TowerBaseClass;};

	int GetGridX() const;
	int GetGridY() const;
	float GetGridSize() const;
protected:
	virtual void BeginPlay() override;

	//	각 그리드 크기에 맞게 싱글그리드들을 배치합니다
	//	원점을 전체 그리드 맵의 모서리로 합니다.
	void PlacingGrid();
	/*void FindSpawnPoints();
	void FindCorePoints();*/

	//	그리드 맵의 X축 칸 수입니다.
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category="TowerGame|Grid")
	int GridX;
	//	그리드 맵의 Y축 칸 수입니다.
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category="TowerGame|Grid")
	int GridY;

	//	각 그리드의 크기입니다
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category="TowerGame|Grid")
	float GridSize;

	UPROPERTY(EditDefaultsOnly, Category="TowerGame|Spawner")
	TSubclassOf<ATGEnemySpawner> EnemySpawnerClass;
	UPROPERTY(EditDefaultsOnly, Category="TowerGame|Spawner")
	TSubclassOf<ATGCoreBase> CoreClass;
	UPROPERTY(EditDefaultsOnly, Category="TowerGame|Grid")
	TSubclassOf<ATGSingleGrid> SingleGridClass;
	UPROPERTY(EditDefaultsOnly, Category="TowerGame|Grid")
	TSubclassOf<ABaseTower> TowerBaseClass;


private:

	bool PathFinding();

	TArray<TArray<bool>> bBuildingPlaced;
	TArray<TArray<ATGSingleGrid*>> GridActors;
	//	진입점 목록
	TSet<FIntPoint> EntryPoints;
	TMap<FIntPoint, TObjectPtr<ATGEnemySpawner>> EnemySpawners;
	//	진출점 목록
	TSet<FIntPoint> ExitPoints;
	TMap<FIntPoint, TObjectPtr<ATGCoreBase>> Cores;

};
