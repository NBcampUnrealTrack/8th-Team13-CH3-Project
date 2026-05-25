#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Core/GameFlow/TGGameFlowState.h"
#include "BaseTower/TGTurretType.h"
#include "TGHUD.generated.h"

class UTGGameOverWidget;
class UTGPlayerWidget;
class UTGPauseWidget;
class UTGBuildWidget;
class ATGGameMode;
class ATGWaveManager;
class ATGEnemyBase;
class ATGPlayer;

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
	void HandleTurretTypeSelected(ETGTurretType SelectedType);

	UFUNCTION()
	void HideAllWidgets(bool bHide);

	void UpdateUIByState(ETGGameFlowState NewState);
	void AddtoViewportPlayerWidget(APlayerController* PC);
	void AddtoViewportBuildWidget(APlayerController* PC);
	void AddtoViewportPausedWidget(APlayerController* PC);
	void AddtoViewportGameOverWidget(APlayerController* PC);
protected:
	UPROPERTY()
	ATGGameMode* CachedGameMode;
	UPROPERTY()
	ATGWaveManager* CachedWaveManager;

	ETGGameFlowState OldState;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UTGPlayerWidget> PlayerWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UTGPauseWidget> PauseWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UTGGameOverWidget> GameOverWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UTGBuildWidget> BuildWidgetClass;
	UPROPERTY()
	UTGPlayerWidget* PlayerWidget;
	UPROPERTY()
	UTGPauseWidget* PauseWidget;
	UPROPERTY()
	UTGGameOverWidget* GameOverWidget;
	UPROPERTY()
	UTGBuildWidget* BuildWidget;

public:
	UTGBuildWidget* GetBuildWidget() const { return BuildWidget; }

private:
	void BindWaveManager();

	UFUNCTION()
	void HandleEnemySpawned(ATGEnemyBase* SpawnedEnemy);

	UFUNCTION()
	void HandleEnemyRemovedForMiniMap(ATGEnemyBase* RemovedEnemy);
};
