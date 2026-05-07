// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TGPlayerWidget.h"
#include "Components/ProgressBar.h"
#include "Player/TGPlayer.h"
#include "Enemies/TGNavigationManager.h"
#include "Enemies/TGCoreBase.h"

void UTGPlayerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	player = Cast<ATGPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
	core = Cast<ATGCoreBase>(ATGNavigationManager::Get(GetWorld())->GetCurrentCoreActor());
}

void UTGPlayerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (player)
		HP_Bar->SetPercent(static_cast<int32>(player->GetPlayerHP() / player->GetPlayerMaxHP()));
	if(core)
		CoreHP_Bar->SetPercent(core->GetCurrentHP() / core->GetMaxHP());
}
