#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TGGameFlowState.h"
#include "TGGameMode.generated.h"

UCLASS()
class TOWERGAME_API ATGGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ATGGameMode();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintReadOnly, Category = "Game Flow")
	ETGGameFlowState CurrentState;

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void ChangeFlowState(ETGGameFlowState NewState);

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void StartGameFlow();

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void EnterBuildMode();

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void ExitBuildMode();

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void PauseGameFlow();

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void ResumeGameFlow();

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void HandleWaveClear();

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void HandleGameOver();

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void HandleGameClear();
};
