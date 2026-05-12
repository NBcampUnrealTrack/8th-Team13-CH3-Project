#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TGTowerWidget.generated.h"

class UTextBlock;

class ATGMountedTower;
class ATGWeaponTower;

UCLASS()
class TOWERGAME_API UTGTowerWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	// 현재 레벨 텍스트
	UPROPERTY(meta = (BindWidget))
	UTextBlock* LevelText;

	// 현재 자원량 텍스트
	UPROPERTY(meta = (BindWidget))
	UTextBlock* EnergyText;

	// 업그레이드 비용 텍스트
	UPROPERTY(meta = (BindWidget))
	UTextBlock* UpgradeCostText;

	// 공격 데미지 텍스트
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DamageText;

	// 공격 사거리 텍스트
	UPROPERTY(meta = (BindWidget))
	UTextBlock* RangeText;

	// 연결된 타워 참조
	UPROPERTY()
	ATGMountedTower* OwnerTower;

public:

	// 타워 연결 및 UI 갱신
	void SetOwnerTower(ATGMountedTower* InTower);

	// UI 갱신
	void RefreshUI();
};
