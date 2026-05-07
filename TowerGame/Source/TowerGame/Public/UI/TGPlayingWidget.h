#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TGPlayingWidget.generated.h"

class UButton;

UCLASS()
class TOWERGAME_API UTGPlayingWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY(meta = (BindWidget))
	UButton* PauseButton;

	UFUNCTION()
	void HandlePauseClicked();
};
