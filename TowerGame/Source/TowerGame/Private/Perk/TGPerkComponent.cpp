// Fill out your copyright notice in the Description page of Project Settings.

#include "Perk/TGPerkComponent.h"
#include "Core/GameFlow/TGGameMode.h"
#include "Core/GameFlow/TGGameFlowState.h"
#include "Player/TGPlayer.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"

UTGPerkComponent::UTGPerkComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTGPerkComponent::BeginPlay()
{
	Super::BeginPlay();

	CachedGameMode = GetGameMode();
	if (CachedGameMode)
	{
		CachedGameMode->OnLevelUp.AddUniqueDynamic(this, &UTGPerkComponent::HandleLevelUp);
		CachedGameMode->OnFlowStateChanged.AddUniqueDynamic(this, &UTGPerkComponent::HandleFlowStateChanged);
	}
}

void UTGPerkComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (CachedGameMode)
	{
		CachedGameMode->OnLevelUp.RemoveDynamic(this, &UTGPerkComponent::HandleLevelUp);
		CachedGameMode->OnFlowStateChanged.RemoveDynamic(this, &UTGPerkComponent::HandleFlowStateChanged);
	}

	Super::EndPlay(EndPlayReason);
}

void UTGPerkComponent::HandleLevelUp(int32 NewLevel)
{
	// 한 번에 여러 레벨이 올라도 빠짐없이 처리되도록 큐에 적재
	PendingLevelUps++;
	TryPresentNext();
}

void UTGPerkComponent::HandleFlowStateChanged(ETGGameFlowState NewState)
{
	// 일시정지/게임오버 등에서 Playing으로 복귀하면 대기 중이던 선택을 재개
	if (NewState == ETGGameFlowState::Playing || NewState == ETGGameFlowState::BuildMode)
	{
		TryPresentNext();
	}
}

void UTGPerkComponent::TryPresentNext()
{
	if (bSelecting) return;
	if (PendingLevelUps <= 0) return;

	ATGGameMode* GM = GetGameMode();
	if (!GM) return;

	// 선택을 띄울 수 있는 상태에서만 진행 (Paused/GameOver 등에서는 복귀 시 재시도)
	const ETGGameFlowState State = GM->CurrentState;
	if (State != ETGGameFlowState::Playing &&
		State != ETGGameFlowState::BuildMode &&
		State != ETGGameFlowState::LevelUp)
	{
		return;
	}

	RollOptions();

	// 제시할 특성이 없으면 소프트락 방지를 위해 대기열을 비우고 종료
	if (CurrentOptions.Num() == 0)
	{
		PendingLevelUps = 0;
		if (GM->CurrentState == ETGGameFlowState::LevelUp)
		{
			GM->ExitLevelUp();
		}
		return;
	}

	bSelecting = true;
	GM->EnterLevelUp();
	OnPerkSelectionPresented.Broadcast(CurrentOptions);
}

void UTGPerkComponent::ConfirmPerkSelection(int32 Index)
{
	if (!bSelecting) return;

	if (CurrentOptions.IsValidIndex(Index))
	{
		ApplyPerk(CurrentOptions[Index]);
	}

	bSelecting = false;
	CurrentOptions.Reset();
	OnPerkSelectionClosed.Broadcast();

	PendingLevelUps = FMath::Max(0, PendingLevelUps - 1);

	if (PendingLevelUps > 0)
	{
		// 남은 레벨업이 있으면 다음 선택을 이어서 제시
		TryPresentNext();
	}
	else if (ATGGameMode* GM = GetGameMode())
	{
		GM->ExitLevelUp();
	}
}

void UTGPerkComponent::RollOptions()
{
	CurrentOptions.Reset();
	if (!PerkTable) return;

	TArray<FTGPerkData*> Rows;
	PerkTable->GetAllRows<FTGPerkData>(TEXT("UTGPerkComponent::RollOptions"), Rows);

	// 가중치 > 0 인 후보만 복사해 풀 구성 (선택 시 풀에서 제거 → 중복 없는 추첨)
	TArray<FTGPerkData> Pool;
	Pool.Reserve(Rows.Num());
	for (const FTGPerkData* Row : Rows)
	{
		if (Row && Row->Weight > 0)
		{
			Pool.Add(*Row);
		}
	}

	const int32 Count = FMath::Min(NumChoices, Pool.Num());
	for (int32 i = 0; i < Count; ++i)
	{
		int32 TotalWeight = 0;
		for (const FTGPerkData& P : Pool)
		{
			TotalWeight += P.Weight;
		}
		if (TotalWeight <= 0) break;

		int32 Roll = FMath::RandRange(0, TotalWeight - 1);
		int32 PickIdx = 0;
		for (; PickIdx < Pool.Num(); ++PickIdx)
		{
			Roll -= Pool[PickIdx].Weight;
			if (Roll < 0) break;
		}
		PickIdx = FMath::Clamp(PickIdx, 0, Pool.Num() - 1);

		CurrentOptions.Add(Pool[PickIdx]);
		Pool.RemoveAt(PickIdx);
	}
}

void UTGPerkComponent::ApplyPerk(const FTGPerkData& Perk)
{
	ATGGameMode* GM = GetGameMode();
	ATGPlayer* Player = GetPlayerPawn();

	switch (Perk.Effect)
	{
	case ETGPerkEffect::MaxHpUp:
		if (Player) Player->ApplyMaxHpDelta(FMath::RoundToInt(Perk.Magnitude));
		break;
	case ETGPerkEffect::Heal:
		if (Player) Player->ChangePlayerHP(FMath::RoundToInt(Perk.Magnitude));
		break;
	case ETGPerkEffect::MoveSpeedUp:
		if (Player) Player->ApplyMoveSpeedMultiplier(Perk.Magnitude);
		break;
	case ETGPerkEffect::WeaponDamageUp:
		if (Player) Player->AddWeaponDamageMultiplier(Perk.Magnitude);
		break;
	case ETGPerkEffect::EvadeCountUp:
		if (Player) Player->AddEvadeCount(FMath::RoundToInt(Perk.Magnitude));
		break;
	case ETGPerkEffect::GrantBuildToken:
		if (GM) GM->AddBuildToken(FMath::RoundToInt(Perk.Magnitude));
		break;
	default:
		break;
	}
}

ATGGameMode* UTGPerkComponent::GetGameMode() const
{
	if (CachedGameMode) return CachedGameMode;
	if (ATGGameMode* OwnerGM = Cast<ATGGameMode>(GetOwner())) return OwnerGM;
	return Cast<ATGGameMode>(UGameplayStatics::GetGameMode(this));
}

ATGPlayer* UTGPerkComponent::GetPlayerPawn() const
{
	return Cast<ATGPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));
}
