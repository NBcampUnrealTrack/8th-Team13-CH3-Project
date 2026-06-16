#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TGGameFlowState.h"
#include "BaseTower/TGTurretType.h"
#include "Engine/StreamableManager.h"
#include "TGGameMode.generated.h"

class USoundBase;
class ATGMountedTower;
class UTGPerkComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTGFlowStateChanged, ETGGameFlowState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnExpChanged, int32, CurrentExp, int32, ExpToNextLevel, int32, CurrentLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelUp, int32, NewLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuildTokenChanged, int32, NewBuildTokens);
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

	// 레벨업 시 무작위 특성 선택을 담당하는 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TowerGame|Perk")
	TObjectPtr<UTGPerkComponent> PerkComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Game Flow")
	ETGGameFlowState OldState;
public:
	UPROPERTY(BlueprintReadOnly, Category = "Game Flow")
	ETGGameFlowState CurrentState;

	UPROPERTY(BlueprintAssignable, Category = "Game Flow")
	FOnTGFlowStateChanged OnFlowStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Game Flow")
	FOnHideWidgets OnHideWidgets;

	// ───────── 경험치 / 레벨 (기존 자원경제 대체) ─────────
	// 레벨 1→2에 필요한 기본 경험치
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TowerGame|Progression")
	int32 BaseExpToNextLevel = 100;
	// 레벨이 오를 때마다 다음 레벨 요구치에 더해지는 증가량
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TowerGame|Progression")
	int32 ExpPerLevelGrowth = 50;

	// 현재 레벨
	UPROPERTY(BlueprintReadOnly, Category = "TowerGame|Progression")
	int32 CurrentLevel = 1;
	// 현재 레벨에서 누적한 경험치
	UPROPERTY(BlueprintReadOnly, Category = "TowerGame|Progression")
	int32 CurrentExp = 0;
	// 다음 레벨까지 필요한 경험치
	UPROPERTY(BlueprintReadOnly, Category = "TowerGame|Progression")
	int32 ExpToNextLevel = 100;

	// 경험치/레벨 변경 델리게이트 (UI 갱신용)
	UPROPERTY(BlueprintAssignable, Category = "TowerGame|Progression")
	FOnExpChanged OnExpChanged;
	// 레벨업 발생 델리게이트 — 특성 선택 시스템(#3)이 이 이벤트를 구독한다
	UPROPERTY(BlueprintAssignable, Category = "TowerGame|Progression")
	FOnLevelUp OnLevelUp;

	// 경험치 획득 (적 처치 등). 누적 후 필요량을 넘으면 자동 레벨업.
	UFUNCTION(BlueprintCallable, Category = "TowerGame|Progression")
	void AddExp(int32 Amount);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TowerGame|Progression")
	int32 GetCurrentLevel() const { return CurrentLevel; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TowerGame|Progression")
	int32 GetCurrentExp() const { return CurrentExp; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TowerGame|Progression")
	int32 GetExpToNextLevel() const { return ExpToNextLevel; }

	// ───────── 건설 토큰 (타워 건설/업그레이드 게이팅) ─────────
	// 게임 시작 시 보유 건설 토큰
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TowerGame|BuildToken")
	int32 StartingBuildTokens = 2;
	// 레벨업 시 지급되는 건설 토큰 (특성 시스템(#3) 도입 전 임시 기본 보상)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TowerGame|BuildToken")
	int32 BuildTokensPerLevel = 1;

	// 현재 보유 건설 토큰
	UPROPERTY(BlueprintReadOnly, Category = "TowerGame|BuildToken")
	int32 BuildTokens = 0;

	// 건설 토큰 변경 델리게이트 (UI 갱신용)
	UPROPERTY(BlueprintAssignable, Category = "TowerGame|BuildToken")
	FOnBuildTokenChanged OnBuildTokenChanged;

	// 건설 토큰 소모, 부족하면 false (건설/업그레이드 차단)
	UFUNCTION(BlueprintCallable, Category = "TowerGame|BuildToken")
	bool TryConsumeBuildToken(int32 Amount = 1);
	// 건설 토큰 추가 (특성 보상, 환불 등)
	UFUNCTION(BlueprintCallable, Category = "TowerGame|BuildToken")
	void AddBuildToken(int32 Amount);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TowerGame|BuildToken")
	int32 GetBuildTokens() const { return BuildTokens; }

	// 레벨업 내부 처리 (경험치 임계치 도달 시 호출)
	void LevelUp();

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

	// 특성 선택 진입/종료 (UTGPerkComponent가 호출, 진입 시 게임 일시정지)
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void EnterLevelUp();

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void ExitLevelUp();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TowerGame|Perk")
	UTGPerkComponent* GetPerkComponent() const { return PerkComponent; }

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
