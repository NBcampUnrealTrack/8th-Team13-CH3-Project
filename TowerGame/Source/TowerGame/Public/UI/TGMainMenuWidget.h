#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TGMainMenuWidget.generated.h"

class UButton;

UCLASS()
class TOWERGAME_API UTGMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UButton* StartButton;

	UFUNCTION()
	void HandleStartClicked();
};
