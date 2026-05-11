#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Core/GameFlow/TGGameFlowState.h"
#include "TGHUD.generated.h"

class UTGGameOverWidget;
class UTGPlayerWidget;
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
	TSubclassOf<UTGPlayerWidget> PlayerWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UTGPauseWidget> PauseWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UTGGameOverWidget> GameOverWidgetClass;
	UPROPERTY()
	UTGPlayerWidget* PlayerWidget;
	UPROPERTY()
	UTGPauseWidget* PauseWidget;
	UPROPERTY()
	UTGGameOverWidget* GameOverWidget;
};
