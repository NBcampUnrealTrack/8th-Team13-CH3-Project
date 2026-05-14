#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TGMainMenuWidget.generated.h"

class UButton;
class UWidget;
class UImage;
class UTexture2D;
class UTextBlock;
class UHorizontalBox;

UCLASS()
class TOWERGAME_API UTGMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> StartButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> EndButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> GuideButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> MainMenuPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> GuidePanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> GuideImage;

	// 가이드 화면 전체 클릭용 투명 버튼
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> GuideClickButton;

	// 밑에 동그라미 표시용 HorizontalBox
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> GuideDotPanel;

	// 가이드 이미지 배열
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Guide")
	TArray<TObjectPtr<UTexture2D>> GuideTextures;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Guide")
	FLinearColor ActiveDotColor = FLinearColor::Black;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Guide")
	FLinearColor InactiveDotColor = FLinearColor(0.35f, 0.35f, 0.35f, 1.0f);

	int32 CurrentGuideIndex = 0;

	TArray<TObjectPtr<UTextBlock>> GuideDotTexts;

	UFUNCTION()
	void HandleStartClicked();

	UFUNCTION()
	void HandleEndClicked();

	UFUNCTION()
	void HandleGuideClicked();

	UFUNCTION()
	void HandleGuidePageClicked();

	void ShowGuide();
	void HideGuide();

	void UpdateGuideImage();
	void RebuildGuideDots();
	void UpdateGuideDots();
};
