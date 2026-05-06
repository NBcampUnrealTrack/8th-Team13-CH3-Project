#include "UI/TGHUD.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Core/GameFlow/TGGameMode.h"

void ATGHUD::BeginPlay()
{
	Super::BeginPlay();

	CachedGameMode = Cast<ATGGameMode>(UGameplayStatics::GetGameMode(this));
	if (CachedGameMode)
	{
		CachedGameMode->OnFlowStateChanged.AddDynamic(this, &ATGHUD::HandleFlowStateChanged);

		UpdateUIByState(CachedGameMode->CurrentState);
	}
}

void ATGHUD::HandleFlowStateChanged(ETGGameFlowState NewState)
{
	UpdateUIByState(NewState);
}

void ATGHUD::HideAllWidgets()
{
	if (MainMenuWidget && MainMenuWidget->IsInViewport())
	{
		MainMenuWidget->RemoveFromParent();
	}
}

void ATGHUD::UpdateUIByState(ETGGameFlowState NewState)
{
	HideAllWidgets();

	APlayerController* PC = GetOwningPlayerController();
	if (!PC)
	{
		return;
	}

	switch (NewState)
	{
	case ETGGameFlowState::Title:
	case ETGGameFlowState::MainMenu:
		if (!MainMenuWidget && MainMenuWidgetClass)
		{
			MainMenuWidget = CreateWidget<UUserWidget>(PC, MainMenuWidgetClass);
		}

		if (MainMenuWidget)
		{
			MainMenuWidget->AddToViewport();
		}
		break;

	default:
		break;
	}
}
