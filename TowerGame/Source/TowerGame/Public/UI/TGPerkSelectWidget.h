// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Perk/TGPerkTypes.h"
#include "TGPerkSelectWidget.generated.h"

class UTGPerkComponent;

// 레벨업 특성 선택 화면. 카드 비주얼은 블루프린트에서 OnShowPerks/OnHidePerks 를 구현해 그린다.
UCLASS()
class TOWERGAME_API UTGPerkSelectWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// 선택지가 제시되면 호출 — BP에서 카드 UI를 생성/표시한다.
	UFUNCTION(BlueprintImplementableEvent, Category = "Perk")
	void OnShowPerks(const TArray<FTGPerkData>& Options);

	// 선택이 끝나면 호출 — BP에서 카드 UI를 닫는다.
	UFUNCTION(BlueprintImplementableEvent, Category = "Perk")
	void OnHidePerks();

	// BP의 카드 버튼에서 호출 — 해당 인덱스의 특성을 확정 선택한다.
	UFUNCTION(BlueprintCallable, Category = "Perk")
	void SelectPerk(int32 Index);

	// 현재 제시 중인 선택지 반환 (BP에서 카드 구성에 사용)
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Perk")
	const TArray<FTGPerkData>& GetOptions() const;

private:
	UFUNCTION()
	void HandlePerkSelectionPresented(const TArray<FTGPerkData>& Options);

	UFUNCTION()
	void HandlePerkSelectionClosed();

	UTGPerkComponent* GetPerkComponent() const;

	UPROPERTY()
	TObjectPtr<UTGPerkComponent> PerkComponent;
};
