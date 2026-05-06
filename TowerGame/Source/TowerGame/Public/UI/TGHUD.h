#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Core/GameFlow/TGGameFlowState.h"
#include "TGHUD.generated.h"

class UTGMainMenuWidget;
class ATGGameMode;

UCLASS()
class TOWERGAME_API ATGHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleFlowStateChanged(ETGGameFlowState NewState);

	void HideAllWidgets();
	void UpdateUIByState(ETGGameFlowState NewState);

protected:
	UPROPERTY()
	ATGGameMode* CachedGameMode = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UTGMainMenuWidget> MainMenuWidgetClass;

	UPROPERTY()
	UTGMainMenuWidget* MainMenuWidget = nullptr;
};
