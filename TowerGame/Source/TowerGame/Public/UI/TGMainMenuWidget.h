#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TGMainMenuWidget.generated.h"

class UButton;
class UWidget;

UCLASS()
class TOWERGAME_API UTGMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	UButton* StartButton;

	UPROPERTY(meta = (BindWidget))
	UButton* EndButton;

	UPROPERTY(meta = (BindWidget))
	UButton* GuideButton;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* BackButton;

	// 블루프린트에서 이름을 정확히 맞춰서 배치
	UPROPERTY(meta = (BindWidgetOptional))
	UWidget* MainMenuPanel;

	UPROPERTY(meta = (BindWidgetOptional))
	UWidget* GuidePanel;

	UFUNCTION()
	void HandleStartClicked();

	UFUNCTION()
	void HandleEndClicked();

	UFUNCTION()
	void HandleGuideClicked();

	UFUNCTION()
	void HandleBackClicked();

	void ShowGuide();
	void HideGuide();
};
