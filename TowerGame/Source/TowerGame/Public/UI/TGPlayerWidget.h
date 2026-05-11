// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TGPlayerWidget.generated.h"

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

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HP_Bar;
	UPROPERTY(meta = (BindWidget))
	UProgressBar* CoreHP_Bar;

	// TODO: UMG 블루프린트(WBP_PlayerWidget)에 TextBlock을 추가하고 이름을 "EnergyText"로 지정하면 자동 바인딩됩니다.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> EnergyText;

	UPROPERTY()
	TObjectPtr<class ATGPlayer> player;

	UPROPERTY()
	TObjectPtr<class ATGCoreBase> core;
};
