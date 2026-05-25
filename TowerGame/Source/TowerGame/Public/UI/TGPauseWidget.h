#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TGPauseWidget.generated.h"

class UButton;

UCLASS()
class TOWERGAME_API UTGPauseWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ResumeButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MainMenuButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitGameButton;

	UFUNCTION()
	void HandleResumeClicked();
	UFUNCTION()
	void HandleMainMenuButtonClicked();
	UFUNCTION()
	void HandleQuitGameClicked();
};
