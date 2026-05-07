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
	UButton* ResumeButton;

	UFUNCTION()
	void HandleResumeClicked();
};
