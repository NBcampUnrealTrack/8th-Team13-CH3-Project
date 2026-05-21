// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TGPlayerWidget.generated.h"

class ATGBossBase;
class ATGEnemyBase;
class ATGWaveManager;
class ATGGameMode;
class ATGNavigationManager;
class UHorizontalBox;
class UProgressBar;
class UButton;
class UTextBlock;
//미니맵 위젯 수정사항
class UTGMiniMapWidget;
class ATGGridBase;
class AActor;
/**
 *
 */
UCLASS()
class TOWERGAME_API UTGPlayerWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UFUNCTION()
	void HandlePauseClicked();

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY()
	TObjectPtr<class ATGPlayer> player;

	UPROPERTY()
	TObjectPtr<class ATGCoreBase> core;

private:
	UPROPERTY()
	TObjectPtr<ATGGameMode> GameMode;

	UPROPERTY()
	TObjectPtr<ATGWaveManager> WaveManager;

	UPROPERTY()
	TObjectPtr<ATGNavigationManager> NavigationManager;

	UPROPERTY()
	TObjectPtr<ATGEnemyBase> FocusedEnemy;

	UPROPERTY()
	TObjectPtr<ATGBossBase> Boss;

	FTimerHandle FocusedEnemyHideTimerHandle;

private:
	// Animation
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_WaveOpacity;

	// TextBlock
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Txt_EnemyType;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Txt_BossName;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Txt_CurrentWave;

	// TODO: UMG 블루프린트(WBP_PlayerWidget)에 TextBlock을 추가하고 이름을 "EnergyText"로 지정하면 자동 바인딩됩니다.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> EnergyText;

	// ProgressBar
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> PB_EnemyHP;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> PB_BossHP;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HP_Bar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> CoreHP_Bar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> Evade_BarGroup;
	UPROPERTY()
	TArray<TObjectPtr<UProgressBar>> Evade_Bars;

private:
	// Focused Enemy 변경 시 호출
	UFUNCTION()
	void HandleFocusedEnemyChanged(ATGEnemyBase* NewEnemy);

	// Enemy 체력 UI에 반영
	UFUNCTION()
	void UpdateFocusedEnemyHPBar(float CurrentHP, float MaxHP);

	// Player 체력 UI에 반영
	UFUNCTION()
	void UpdatePlayerHPBar(float CurrentHP, float MaxHP);

	// Player 회피게이지 UI에 반영
	UFUNCTION()
	void UpdateEvadeBar(int32 CurrentEvadeCount, float CooldownRate);

	// Enemy 제거 시 호출
	UFUNCTION()
	void HandleFocusedEnemyRemoved(ATGEnemyBase* RemovedEnemy);

	// 각 Wave 시작 시 호출
	UFUNCTION()
	void HandleWaveStarted(int32 WaveIndex);

	// 보스 생성 시 호출
	UFUNCTION()
	void BindBoss(ATGBossBase* NewBoss);

	// Boss HP 변동 시 호출
	UFUNCTION()
	void UpdateBossHPBar(float CurrentHP, float MaxHP);

	// Boss 제거 시 호출
	UFUNCTION()
	void HandleBossRemoved(ATGBossBase* RemovedBoss);

	// 현재 코어 변경 시 호출
	UFUNCTION()
	void UpdateCurrentCore(ATGCoreBase* NewCore);

	// Core Hp 변경 시 호출
	UFUNCTION()
	void HandleCoreHPChanged(float CurrentHP, float MaxHP);

	// 자원 변경 시 호출
	UFUNCTION()
	void HandleEnergyChanged(int32 NewEnergy);

	void HideFocusedEnemyInfo();

	// UI에 연결된 enemy 관리
	void BindFocusedEnemy(ATGEnemyBase* NewEnemy);
	void UnbindFocusedEnemy();

	// UI에 연결된 Boss 관리
	void UnbindBoss();
//미니맵 위젯
public:
	void SetGridBase(ATGGridBase* InGridBase);
	void RegisterMonsterToMiniMap(AActor* MonsterActor);
	void UnregisterMonsterFromMiniMap(AActor* MonsterActor);
protected:
	UPROPERTY(meta = (BindWidget))
	UTGMiniMapWidget* MiniMapWidget;
};
