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
class UTextBlock;

/**
 * 플레이 중 단일 HUD 위젯.
 * 플레이어/코어 체력 상시 표시 + 경험치/레벨/건설토큰 + (보스/포커스 적 체력) + 건설 퀵슬롯.
 * 미니맵/현재 총기 표시는 제거됨.
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

	// 진행도(레벨/경험치) / 건설토큰
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> LevelText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> PB_Exp;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> BuildTokenText;

	// ProgressBar
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> PB_EnemyHP;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> PB_BossHP;

	// 상시 표시 — 플레이어 / 코어 체력 (위젯 재설계 자유도를 위해 Optional + null 가드)
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> HP_Bar;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> CoreHP_Bar;
	UPROPERTY(meta = (BindWidgetOptional))
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

	// 경험치/레벨 변경 시 호출
	UFUNCTION()
	void HandleExpChanged(int32 CurrentExp, int32 ExpToNextLevel, int32 CurrentLevel);

	// 건설 토큰 변경 시 호출
	UFUNCTION()
	void HandleBuildTokenChanged(int32 NewBuildTokens);

	// 건설 퀵슬롯 변경 시 호출 → BP의 슬롯바 갱신
	UFUNCTION()
	void HandleBuildSlotChanged(int32 SelectedIndex, int32 SlotCount);

	UFUNCTION()
	void HideFocusedEnemyInfo();

	// UI에 연결된 enemy 관리
	void BindFocusedEnemy(ATGEnemyBase* NewEnemy);
	void UnbindFocusedEnemy();

	// UI에 연결된 Boss 관리
	void UnbindBoss();

protected:
	// BP에서 구현 — 하단 퀵슬롯 바 갱신(1=미선택, 2=벽, 3+=타워) 및 선택 강조
	UFUNCTION(BlueprintImplementableEvent, Category = "Build")
	void OnBuildSlotUpdated(int32 SelectedIndex, int32 SlotCount);
};
