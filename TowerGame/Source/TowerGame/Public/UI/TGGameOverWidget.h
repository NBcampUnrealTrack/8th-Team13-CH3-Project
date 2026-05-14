#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TGGameOverWidget.generated.h"

class UButton;

UCLASS()
class TOWERGAME_API UTGGameOverWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	UButton* ResultButton;
	UPROPERTY(meta = (BindWidget))
	UButton* MainMenuButton;
	UPROPERTY(meta = (BindWidgetOptional))
	UWidget* GameOverPanel;
	UPROPERTY(meta = (BindWidgetOptional))
	UWidget* ResultPanel;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeInDark;

	UFUNCTION()
	void HandleResultClicked();

	UFUNCTION()
	void HandleMainMenuClicked();

	void ShowResult();
	void HideResult();

	void PlayGameOverAnimation();
};
