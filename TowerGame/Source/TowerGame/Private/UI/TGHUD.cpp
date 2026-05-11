#include "UI/TGHUD.h"
#include "UI/TGMainMenuWidget.h"
#include "UI/TGGameOverWidget.h"
#include "UI/TGPlayerWidget.h"
#include "UI/TGPlayingWidget.h"
#include "UI/TGPauseWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Core/GameFlow/TGGameMode.h"

ATGHUD::ATGHUD()
	: CachedGameMode(nullptr),
	PlayerWidget(nullptr),
	PauseWidget(nullptr),
	GameOverWidget(nullptr)
{
}

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
	UE_LOG(LogTemp, Warning, TEXT("HUD State Changed: %s"), *UEnum::GetValueAsString(NewState));
	UpdateUIByState(NewState);
}

void ATGHUD::HideAllWidgets()
{
	if (PlayerWidget && PlayerWidget->IsInViewport())
	{
		PlayerWidget->RemoveFromParent();
	}
	if (PauseWidget && PauseWidget->IsInViewport())
	{
		PauseWidget->RemoveFromParent();
	}

	if (GameOverWidget && GameOverWidget->IsInViewport())
	{
		GameOverWidget->RemoveFromParent();
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
	case ETGGameFlowState::Playing:
		if (!PlayerWidget && PlayerWidgetClass)
		{
			PlayerWidget = CreateWidget<UTGPlayerWidget>(PC, PlayerWidgetClass);
		}

		if (PlayerWidget)
		{
			PlayerWidget->AddToViewport();
		}

		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
		break;

	case ETGGameFlowState::Paused:
		if (!PauseWidget && PauseWidgetClass)
		{
			PauseWidget = CreateWidget<UTGPauseWidget>(PC, PauseWidgetClass);
		}

		if (PauseWidget)
		{
			PauseWidget->AddToViewport();
		}

		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeUIOnly());
		break;

	case ETGGameFlowState::GameOver:
		if (!GameOverWidget && GameOverWidgetClass)
		{
			GameOverWidget = CreateWidget<UTGGameOverWidget>(PC, GameOverWidgetClass);
		}

		if (GameOverWidget)
		{
			GameOverWidget->AddToViewport();
		}

		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeUIOnly());
		break;

	default:
		break;
	}
}
