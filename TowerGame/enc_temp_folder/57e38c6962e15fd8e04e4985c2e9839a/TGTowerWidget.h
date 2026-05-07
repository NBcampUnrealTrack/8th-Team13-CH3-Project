#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TGTowerWidget.generated.h"

class UTextBlock;
class UButton;
class ATGMountedTower;

UCLASS()
class TOWERGAME_API UTGTowerWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	// 현재 레벨 텍스트
	UPROPERTY(meta = (BindWidget))
	UTextBlock* LevelText;

	// 업그레이드 버튼
	UPROPERTY(meta = (BindWidget))
	UButton* UpgradeButton;

	// 연결된 타워 참조
	UPROPERTY()
	ATGMountedTower* OwnerTower;

	UFUNCTION()
	void HandleUpgradeClicked();

public:
	// 타워 연결 및 UI 갱신
	void SetOwnerTower(ATGMountedTower* InTower);

	// UI 갱신
	void RefreshUI();
};
