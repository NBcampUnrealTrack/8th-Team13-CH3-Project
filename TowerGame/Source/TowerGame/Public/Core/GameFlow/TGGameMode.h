#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TGGameFlowState.h"
#include "TGGameMode.generated.h"

//class ATGCoreBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTGFlowStateChanged, ETGGameFlowState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTGEnergyChanged, int32, NewEnergy);

UCLASS()
class TOWERGAME_API ATGGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ATGGameMode();

protected:
	virtual void BeginPlay() override;

	FTimerHandle WaveStartTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Flow")
	float SetWaveStartTime;

	//UFUNCTION()
	//void OnCoreDestroyedFromDelegate();

	//UPROPERTY()
	//ATGCoreBase* CachedCore = nullptr;

public:
	UPROPERTY(BlueprintReadOnly, Category = "Game Flow")
	ETGGameFlowState CurrentState;

	UPROPERTY(BlueprintAssignable, Category = "Game Flow")
	FOnTGFlowStateChanged OnFlowStateChanged;

	UPROPERTY(BlueprintReadOnly, Category = "Game|Economy")
	int32 CurrentEnergy;

	UPROPERTY(BlueprintAssignable, Category = "Game|Economy")
	FOnTGEnergyChanged OnEnergyChanged;

	UFUNCTION(BlueprintCallable, Category = "Game|Economy")
	bool SpendEnergy(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Game|Economy")
	void AddEnergy(int32 Amount);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Game|Economy")
	int32 GetEnergy() const;

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

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void StartWave();

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	bool IsPauseGameFlow();
};
