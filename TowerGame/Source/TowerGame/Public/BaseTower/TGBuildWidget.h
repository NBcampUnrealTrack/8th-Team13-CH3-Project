#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseTower/TGTurretType.h"
#include "TGBuildWidget.generated.h"

class UImage;

UCLASS()
class TOWERGAME_API UTGBuildWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	// 슬롯 이미지 (4종) — BP에서 이름 일치
	UPROPERTY(meta = (BindWidget))
	UImage* Image_Slot1;

	UPROPERTY(meta = (BindWidget))
	UImage* Image_Slot2;

	UPROPERTY(meta = (BindWidget))
	UImage* Image_Slot3;

	UPROPERTY(meta = (BindWidget))
	UImage* Image_Slot4;

	// 선택된 슬롯 색상 (밝게)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot|Style")
	FLinearColor SelectedColor = FLinearColor::White;

	// 미선택 슬롯 색상 (어둡게)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot|Style")
	FLinearColor NormalColor = FLinearColor(0.4f, 0.4f, 0.4f, 0.6f);

public:
	// 선택된 타워 타입에 맞춰 슬롯 이미지 강조 갱신
	void RefreshSlotHighlight(ETGTurretType SelectedType);
};
