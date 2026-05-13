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

	// Core 변경 이벤트를 구독하고, 코어 수동 초기화 1회 진행
	NavigationManager = ATGNavigationManager::Get(this);
	if (NavigationManager){
		NavigationManager->OnCurrentCoreChanged.AddDynamic(this, &UTGPlayerWidget::UpdateCurrentCore);
		UpdateCurrentCore(NavigationManager->GetCurrentCoreActor());
	}

	// 자원 변경 이벤트를 구독하고, 자원 수동 초기화 1회 진행
	GameMode = Cast<ATGGameMode>(UGameplayStatics::GetGameMode(this));
	if(GameMode){
		GameMode->OnEnergyChanged.AddUniqueDynamic(this, &UTGPlayerWidget::HandleEnergyChanged);

		HandleEnergyChanged(GameMode->GetCurrentEnergy());
	}
}

void UTGPlayerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (player)
	{
		HP_Bar->SetPercent(player->GetPlayerHP() / player->GetPlayerMaxHP());
	}

}

void UTGPlayerWidget::NativeDestruct()
{
	// 구독했던 델리게이트들 해제
	if (NavigationManager){
		NavigationManager->OnCurrentCoreChanged.RemoveDynamic(this, &UTGPlayerWidget::UpdateCurrentCore);
	}

	if (core){
		core->OnCoreHPChanged.RemoveDynamic(this, &UTGPlayerWidget::HandleCoreHPChanged);
		core = nullptr;
	}

	if (GameMode){
		GameMode->OnEnergyChanged.RemoveDynamic(this, &UTGPlayerWidget::HandleEnergyChanged);
	}

	Super::NativeDestruct();
}

void UTGPlayerWidget::UpdateCurrentCore(ATGCoreBase* NewCore)
{
	// NewCore가 변하지 않은 상태에서의 중복 호출 방어 코드
	if (core == NewCore){
		if (core){
			HandleCoreHPChanged(core->GetCurrentHP(), core->GetMaxHP());
		}
		return;
	}

	// 기존 델리게이트 구독 해제
	if (core){
		core->OnCoreHPChanged.RemoveDynamic(this, &UTGPlayerWidget::HandleCoreHPChanged);
	}

	// Core HP 변경 이벤트 구독 및 수동 초기화 1회 진행
	core = NewCore;
	if (core){
		core->OnCoreHPChanged.AddUniqueDynamic(this, &UTGPlayerWidget::HandleCoreHPChanged);

		HandleCoreHPChanged(core->GetCurrentHP(), core->GetMaxHP());
	}else if (CoreHP_Bar){
		CoreHP_Bar->SetPercent(0);
	}
}

void UTGPlayerWidget::HandleCoreHPChanged(float CurrentHP, float MaxHP)
{
	if (!CoreHP_Bar) return;

	const float Percent = CurrentHP / MaxHP;
	CoreHP_Bar->SetPercent(FMath::Clamp(Percent, 0.0f, 1.0f));
}

void UTGPlayerWidget::HandleEnergyChanged(int32 NewEnergy)
{
	if (!EnergyText) return;

	EnergyText->SetText(FText::AsNumber(NewEnergy));

}

void UTGPlayerWidget::HandlePauseClicked()
{
	if (ATGGameMode* GM = Cast<ATGGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->PauseGameFlow();
	}
}
