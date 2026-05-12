// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TGPlayerWidget.h"
#include "Components/ProgressBar.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Player/TGPlayer.h"
#include "Enemies/TGNavigationManager.h"
#include "Enemies/TGCoreBase.h"
#include "Kismet/GameplayStatics.h"
#include "Core/GameFlow/TGGameMode.h"

void UTGPlayerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	player = Cast<ATGPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());

	if (ATGGameMode* GM = Cast<ATGGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		if (EnergyText)
			EnergyText->SetText(FText::AsNumber(GM->GetCurrentEnergy()));
	}
}

void UTGPlayerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (player)
		HP_Bar->SetPercent(static_cast<int32>(player->GetPlayerHP() / player->GetPlayerMaxHP()));
	core = Cast<ATGCoreBase>(ATGNavigationManager::Get(GetWorld())->GetCurrentCoreActor());
	if (core)
		CoreHP_Bar->SetPercent(core->GetCurrentHP() / core->GetMaxHP());
	if (EnergyText)
	{
		if (ATGGameMode* GM = Cast<ATGGameMode>(UGameplayStatics::GetGameMode(this)))
			EnergyText->SetText(FText::AsNumber(GM->GetCurrentEnergy()));
	}
}

void UTGPlayerWidget::HandlePauseClicked()
{
	if (ATGGameMode* GM = Cast<ATGGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->PauseGameFlow();
	}
}
