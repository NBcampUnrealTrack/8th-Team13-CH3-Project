#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Core/GameFlow/TGGameFlowState.h"
#include "TGHUD.generated.h"

class UUserWidget;
class ATGGameMode;

UCLASS()
class TOWERGAME_API ATGHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	void HandleFlowStateChanged(ETGGameFlowState NewState);

	UFUNCTION(BlueprintCallable)
	void UpdateUIByState(ETGGameFlowState NewState);

protected:
	void HideAllWidgets();

	UPROPERTY()
	TObjectPtr<ATGGameMode> CachedGameMode;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> MainMenuWidget;
};
