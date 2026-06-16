#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Core/GameFlow/TGGameFlowState.h"
#include "TGHUD.generated.h"

class UTGGameOverWidget;
class UTGPlayerWidget;
class UTGPauseWidget;
class UTGPerkSelectWidget;
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

	UFUNCTION()
	void HideAllWidgets();

	void UpdateUIByState(ETGGameFlowState NewState);
	void AddtoViewportPlayerWidget(APlayerController* PC);
	void AddtoViewportPausedWidget(APlayerController* PC);
	void AddtoViewportGameOverWidget(APlayerController* PC);
	void AddtoViewportPerkWidget(APlayerController* PC);

protected:
	UPROPERTY()
	ATGGameMode* CachedGameMode;

	ETGGameFlowState OldState;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UTGPlayerWidget> PlayerWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UTGPauseWidget> PauseWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UTGGameOverWidget> GameOverWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UTGPerkSelectWidget> PerkWidgetClass;

	UPROPERTY()
	UTGPlayerWidget* PlayerWidget;
	UPROPERTY()
	UTGPauseWidget* PauseWidget;
	UPROPERTY()
	UTGGameOverWidget* GameOverWidget;
	UPROPERTY()
	UTGPerkSelectWidget* PerkWidget;
};
