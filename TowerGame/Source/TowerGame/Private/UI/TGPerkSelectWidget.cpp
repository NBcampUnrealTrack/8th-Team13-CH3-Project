// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/TGPerkSelectWidget.h"
#include "Perk/TGPerkComponent.h"
#include "Core/GameFlow/TGGameMode.h"
#include "Kismet/GameplayStatics.h"

void UTGPerkSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PerkComponent = GetPerkComponent();
	if (PerkComponent)
	{
		PerkComponent->OnPerkSelectionPresented.AddUniqueDynamic(this, &UTGPerkSelectWidget::HandlePerkSelectionPresented);
		PerkComponent->OnPerkSelectionClosed.AddUniqueDynamic(this, &UTGPerkSelectWidget::HandlePerkSelectionClosed);

		// 위젯 생성 시점에 이미 선택지가 제시된 경우(이벤트를 놓친 경우) 즉시 표시
		if (PerkComponent->IsSelecting() && PerkComponent->GetCurrentOptions().Num() > 0)
		{
			OnShowPerks(PerkComponent->GetCurrentOptions());
		}
	}
}

void UTGPerkSelectWidget::NativeDestruct()
{
	if (PerkComponent)
	{
		PerkComponent->OnPerkSelectionPresented.RemoveDynamic(this, &UTGPerkSelectWidget::HandlePerkSelectionPresented);
		PerkComponent->OnPerkSelectionClosed.RemoveDynamic(this, &UTGPerkSelectWidget::HandlePerkSelectionClosed);
	}

	Super::NativeDestruct();
}

void UTGPerkSelectWidget::HandlePerkSelectionPresented(const TArray<FTGPerkData>& Options)
{
	OnShowPerks(Options);
}

void UTGPerkSelectWidget::HandlePerkSelectionClosed()
{
	OnHidePerks();
}

void UTGPerkSelectWidget::SelectPerk(int32 Index)
{
	if (PerkComponent)
	{
		PerkComponent->ConfirmPerkSelection(Index);
	}
}

const TArray<FTGPerkData>& UTGPerkSelectWidget::GetOptions() const
{
	static const TArray<FTGPerkData> Empty;
	return PerkComponent ? PerkComponent->GetCurrentOptions() : Empty;
}

UTGPerkComponent* UTGPerkSelectWidget::GetPerkComponent() const
{
	if (ATGGameMode* GM = Cast<ATGGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		return GM->GetPerkComponent();
	}
	return nullptr;
}
