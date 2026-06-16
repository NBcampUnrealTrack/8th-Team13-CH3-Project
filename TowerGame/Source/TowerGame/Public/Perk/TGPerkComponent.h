// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Perk/TGPerkTypes.h"
#include "Core/GameFlow/TGGameFlowState.h"
#include "TGPerkComponent.generated.h"

class ATGGameMode;
class ATGPlayer;
class UDataTable;

// 특성 선택지 제시 이벤트 (UI가 구독해 카드를 띄움)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPerkSelectionPresented, const TArray<FTGPerkData>&, Options);
// 특성 선택 종료 이벤트 (UI가 구독해 카드를 닫음)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPerkSelectionClosed);

// 레벨업 시 무작위 특성 3택1 선택을 담당하는 컴포넌트. GameMode에 부착해 사용한다.
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TOWERGAME_API UTGPerkComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTGPerkComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// 특성 카탈로그 (Row Struct = FTGPerkData)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Perk")
	TObjectPtr<UDataTable> PerkTable;

	// 한 번에 제시할 특성 개수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Perk", meta = (ClampMin = "1"))
	int32 NumChoices = 3;

	// 선택지 제시 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Perk")
	FOnPerkSelectionPresented OnPerkSelectionPresented;

	// 선택 종료 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Perk")
	FOnPerkSelectionClosed OnPerkSelectionClosed;

	// 현재 제시 중인 선택지
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Perk")
	const TArray<FTGPerkData>& GetCurrentOptions() const { return CurrentOptions; }

	// 현재 선택 진행 중인지
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Perk")
	bool IsSelecting() const { return bSelecting; }

	// UI에서 특성을 확정 선택할 때 호출 (Index = 선택지 인덱스)
	UFUNCTION(BlueprintCallable, Category = "Perk")
	void ConfirmPerkSelection(int32 Index);

private:
	// GameMode 레벨업 이벤트 처리
	UFUNCTION()
	void HandleLevelUp(int32 NewLevel);

	// 플로우 상태 변경 처리 (일시정지 해제 후 대기 중 선택 재개)
	UFUNCTION()
	void HandleFlowStateChanged(ETGGameFlowState NewState);

	// 대기 중인 선택을 가능한 시점에 제시
	void TryPresentNext();
	// 가중치 기반 무작위 선택지 생성
	void RollOptions();
	// 선택된 특성 효과 적용
	void ApplyPerk(const FTGPerkData& Perk);

	ATGGameMode* GetGameMode() const;
	ATGPlayer* GetPlayerPawn() const;

private:
	UPROPERTY()
	TObjectPtr<ATGGameMode> CachedGameMode;

	// 처리 대기 중인 레벨업 횟수 (한 번에 여러 레벨 상승 가능)
	int32 PendingLevelUps = 0;

	// 현재 선택 화면이 떠 있는지
	bool bSelecting = false;

	// 현재 제시 중인 선택지
	UPROPERTY()
	TArray<FTGPerkData> CurrentOptions;
};
