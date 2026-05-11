#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TGMenuHUD.generated.h"

class UTGMainMenuWidget;

UCLASS()
class TOWERGAME_API ATGMenuHUD : public AHUD
{
	GENERATED_BODY()
protected:
	ATGMenuHUD();
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UTGMainMenuWidget> MainMenuWidgetClass;

	UPROPERTY()
	UTGMainMenuWidget* MainMenuWidget;
};
