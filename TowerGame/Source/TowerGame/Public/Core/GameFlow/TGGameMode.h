#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TGGameFlowState.h"
#include "BaseTower/TGTurretType.h"
#include "Engine/StreamableManager.h"
#include "TGGameMode.generated.h"

class USoundBase;
class ATGMountedTower;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTGFlowStateChanged, ETGGameFlowState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnergyChanged, int32, NewEnergy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHideWidgets);

UCLASS()
class TOWERGAME_API ATGGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ATGGameMode();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void FinishGameOver();

	FTimerHandle WaveStartTimerHandle;
	FTimerHandle GameOverTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Flow")
	float SetWaveStartTime;

	// 플레이 배경음 에셋
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BGM")
	USoundBase* GameBGM;

	// 레벨 로드 시 미리 로드할 에셋 목록
	UPROPERTY(EditAnywhere, Category = "Preload")
	TArray<TSoftObjectPtr<UObject>> PreloadAssets;

	// 로드 핸들 — 멤버로 유지해야 에셋이 GC되지 않음
	TSharedPtr<FStreamableHandle> PreloadHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TowerGame")
	TMap<ETGTurretType, TSubclassOf<ATGMountedTower>> TowerMap;

	UPROPERTY(BlueprintReadOnly, Category = "Game Flow")
	ETGGameFlowState OldState;
public:
	UPROPERTY(BlueprintReadOnly, Category = "Game Flow")
	ETGGameFlowState CurrentState;

	UPROPERTY(BlueprintAssignable, Category = "Game Flow")
	FOnTGFlowStateChanged OnFlowStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Game Flow")
	FOnHideWidgets OnHideWidgets;

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
	virtual void StartGameFlow();

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void EnterBuildMode();

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void ExitBuildMode();

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void PauseGameFlow();

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void ResumeGameFlow();

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void HandleGameOver();

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void HandleGameClear();

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void StartWave();

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	bool IsPauseGameFlow();

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void BackMainMenu();

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void EndGame();
};
