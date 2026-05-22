// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseTower/TGTurretType.h"
#include "TGInteractiveActor.h"
#include "TGSingleGrid.generated.h"

class ABaseTower;
class ATGGridBase;

UCLASS()
class TOWERGAME_API ATGSingleGrid : public ATGInteractiveActor
{
	GENERATED_BODY()
	//	그리드 위에 배치되는 최소 단위 클래스입니다.
	//	개별 칸 관리에 사용됩니다.
	//	TODO: 런타임에 감시가 필요한 타워 데이터에 대해 저장합니다.
public:
	ATGSingleGrid();

	void SetParent(TObjectPtr<ATGGridBase> Parent);

	void SetBoxSize(float Size) const;

	// 그리드 위의 건물 파괴하고 초기상태로 되돌림.
	void ResetGrid();
	// void PlaceTower(TObjectPtr<ABaseTower> Tower);
	// void RemoveTower(TObjectPtr<ABaseTower> Tower);

	virtual void OnFocused_Implementation(ATGPlayer* Player) override;
	virtual void OnUnfocused_Implementation(ATGPlayer* Player) override;
	virtual void OnInteract_Implementation(ATGPlayer* Player) override;

	// 미니맵 데이터 전달용 함수
public:
	UFUNCTION()
	void HandleMountedTowerInstalled(ETGTurretType TurretType);
protected:

	virtual void BeginPlay() override;

	//	월드에 위치를 표시하기 위한 임시 컴포넌트입니다.
	//	ToDo : 디버깅용 표시 방법이 없어도 될 때 삭제하세요.
	UPROPERTY(EditDefaultsOnly, Category = "TowerGame|Grid")
	TObjectPtr<UStaticMeshComponent> Visualizer;

	UPROPERTY()
	TSubclassOf<ABaseTower> BaseTowerClass;

private:
	//	그리드 매니저
	UPROPERTY()
	TObjectPtr<ATGGridBase> GridBase = nullptr;

	UPROPERTY()
	TObjectPtr<ABaseTower> PlacedTower = nullptr;

	FIntPoint MyPoint;
	bool bIsPlacedTower = false;
};
