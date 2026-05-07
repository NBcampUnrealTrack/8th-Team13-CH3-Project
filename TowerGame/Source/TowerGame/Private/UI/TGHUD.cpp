#include "UI/TGHUD.h"
#include "UI/TGMainMenuWidget.h"
#include "UI/TGGameOverWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Core/GameFlow/TGGameMode.h"

ATGHUD::ATGHUD()
	: CachedGameMode(nullptr),
	MainMenuWidget(nullptr),
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
	if (MainMenuWidget && MainMenuWidget->IsInViewport())
	{
		MainMenuWidget->RemoveFromParent();
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
	case ETGGameFlowState::Title:
	case ETGGameFlowState::MainMenu:
		if (!MainMenuWidget && MainMenuWidgetClass)
		{
			MainMenuWidget = CreateWidget<UTGMainMenuWidget>(PC, MainMenuWidgetClass);
		}

		if (MainMenuWidget)
		{
			MainMenuWidget->AddToViewport();
		}
		break;

	case ETGGameFlowState::Playing:
		UE_LOG(LogTemp, Warning, TEXT("Now Playing"));
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
		break;

	default:
		break;
	}
}
