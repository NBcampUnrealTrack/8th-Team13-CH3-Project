#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TGBuildWidget.generated.h"

class UTextBlock;
class ABaseTower;

UCLASS()
class TOWERGAME_API UTGBuildWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	// 슬롯 1 : BaseTower
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Slot1Name;

	// 슬롯 2 : WeaponTower
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Slot2Name;

	// 슬롯 3 : DebuffTower
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Slot3Name;

	// 슬롯 4 : BuffTower
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Slot4Name;

	// 현재 보유 에너지
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Energy;

	UPROPERTY()
	ABaseTower* OwnerTower;
public:
	// 타워 연결 및 UI 갱신
	void SetOwnerTower(ABaseTower* InTower);

	// UI 갱신
	void RefreshUI();
};
