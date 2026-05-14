// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TGPlayerWidget.generated.h"

class ATGEnemyBase;
class ATGWaveManager;
class ATGGameMode;
class ATGNavigationManager;
class UProgressBar;
class UButton;
class UTextBlock;

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
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
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

	FTimerHandle FocusedEnemyHideTimerHandle;

private:
	// Animation
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_WaveOpacity;

	// TextBlock
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Txt_EnemyType;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Txt_CurrentWave;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Txt_RemainEnemy;

	// TODO: UMG 블루프린트(WBP_PlayerWidget)에 TextBlock을 추가하고 이름을 "EnergyText"로 지정하면 자동 바인딩됩니다.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> EnergyText;

	// ProgressBar
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> PB_EnemyHP;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HP_Bar;
	UPROPERTY(meta = (BindWidget))
	UProgressBar* CoreHP_Bar;

private:
	// Focused Enemy 변경 시 호출
	UFUNCTION()
	void HandleFocusedEnemyChanged(ATGEnemyBase* NewEnemy);

	// 각 Wave 시작 시 호출
	UFUNCTION()
	void HandleWaveStarted(int32 WaveIndex);

	// 적 수 변경 시 호출
	UFUNCTION()
	void HandleAliveEnemyCountChanged(int32 AliveEnemyCount);

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

};
