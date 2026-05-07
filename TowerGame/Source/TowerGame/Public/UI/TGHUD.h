#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Core/GameFlow/TGGameFlowState.h"
#include "TGHUD.generated.h"

class UTGMainMenuWidget;
class UTGGameOverWidget;
class UTGPlayerWidget;
class UTGPlayingWidget;
class UTGPauseWidget;
class ATGGameMode;

UCLASS()
class TOWERGAME_API ATGHUD : public AHUD
{
	GENERATED_BODY()

protected:
	ATGHUD();
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleFlowStateChanged(ETGGameFlowState NewState);

	void HideAllWidgets();
	void UpdateUIByState(ETGGameFlowState NewState);

protected:
	UPROPERTY()
	ATGGameMode* CachedGameMode;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UTGMainMenuWidget> MainMenuWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UTGPlayerWidget> PlayerWidgetClass;
	//UPROPERTY(EditDefaultsOnly, Category = "UI")
	//TSubclassOf<UTGPlayingWidget> PlayingWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UTGPauseWidget> PauseWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UTGGameOverWidget> GameOverWidgetClass;
	UPROPERTY()
	UTGMainMenuWidget* MainMenuWidget;
	UPROPERTY()
	UTGPlayerWidget* PlayerWidget;
	//UPROPERTY()
	//UTGPlayingWidget* PlayingWidget;
	UPROPERTY()
	UTGPauseWidget* PauseWidget;
	UPROPERTY()
	UTGGameOverWidget* GameOverWidget;
};
