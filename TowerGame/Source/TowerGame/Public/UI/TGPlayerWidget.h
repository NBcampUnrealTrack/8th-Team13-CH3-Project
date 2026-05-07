// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TGPlayerWidget.generated.h"

class UProgressBar;

/**
 * 
 */
UCLASS()
class TOWERGAME_API UTGPlayerWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HP_Bar;
	UPROPERTY(meta = (BindWidget))
	UProgressBar* CoreHP_Bar;

	UPROPERTY()
	TObjectPtr<class ATGPlayer> player;

	UPROPERTY()
	TObjectPtr<class ATGCoreBase> core;
};
