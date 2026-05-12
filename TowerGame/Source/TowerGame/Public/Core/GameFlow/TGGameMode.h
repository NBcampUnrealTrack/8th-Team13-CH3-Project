#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TGGameFlowState.h"
#include "BaseTower/TGTurretType.h"
#include "TGGameMode.generated.h"

//class ATGCoreBase;

class ATGMountedTower;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTGFlowStateChanged, ETGGameFlowState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnergyChanged, int32, NewEnergy);

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TowerGame")
	TMap<ETGTurretType, TSubclassOf<ATGMountedTower>> TowerMap;

	//UFUNCTION()
	//void OnCoreDestroyedFromDelegate();

	//UPROPERTY()
	//ATGCoreBase* CachedCore = nullptr;

public:
	UPROPERTY(BlueprintReadOnly, Category = "Game Flow")
	ETGGameFlowState CurrentState;

	UPROPERTY(BlueprintAssignable, Category = "Game Flow")
	FOnTGFlowStateChanged OnFlowStateChanged;

	//	현재 자원량
	UPROPERTY(BlueprintReadOnly, Category = "TowerGame|Economy")
	int32 CurrentEnergy;
	//	자원 정보 변경 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "TowerGame|Economy")
	FOnEnergyChanged OnEnergyChanged;

	//	자원 관련
	//	자원 사용, 실패시 사용 안됨
	UFUNCTION(BlueprintCallable, Category = "TowerGame|Economy")
	bool SpendEnergy(int32 Amount);
	//	자원 추가
	UFUNCTION(BlueprintCallable, Category = "TowerGame|Economy")
	void AddEnergy(int32 Amount);
	//	현재 자원량을 반환합니다
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TowerGame|Economy")
	int32 GetCurrentEnergy() const;

	UFUNCTION(BlueprintCallable, Category = "TowerGame|Tower")
	TSubclassOf<ATGMountedTower> GetTowerSubclass(ETGTurretType type);

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
