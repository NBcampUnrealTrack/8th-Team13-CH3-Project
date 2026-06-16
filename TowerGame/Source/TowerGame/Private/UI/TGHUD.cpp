#include "UI/TGHUD.h"
#include "UI/TGGameOverWidget.h"
#include "UI/TGPlayerWidget.h"
#include "UI/TGPauseWidget.h"
#include "UI/TGPerkSelectWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Core/GameFlow/TGGameMode.h"

ATGHUD::ATGHUD()
	: CachedGameMode(nullptr),
	PlayerWidget(nullptr),
	PauseWidget(nullptr),
	GameOverWidget(nullptr),
	PerkWidget(nullptr)
{
}

void ATGHUD::BeginPlay()
{
	Super::BeginPlay();

	CachedGameMode = Cast<ATGGameMode>(UGameplayStatics::GetGameMode(this));
	if (CachedGameMode)
	{
		CachedGameMode->OnFlowStateChanged.AddDynamic(this, &ATGHUD::HandleFlowStateChanged);
		CachedGameMode->OnHideWidgets.AddDynamic(this, &ATGHUD::HideAllWidgets);

		UpdateUIByState(CachedGameMode->CurrentState);
	}
}

void ATGHUD::HandleFlowStateChanged(ETGGameFlowState NewState)
{
	UE_LOG(LogTemp, Log, TEXT("HUD State Changed: %s"), *UEnum::GetValueAsString(NewState));
	UpdateUIByState(NewState);
}

void ATGHUD::HideAllWidgets()
{
	if (PlayerWidget && PlayerWidget->IsInViewport())
	{
		PlayerWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	if (PauseWidget && PauseWidget->IsInViewport())
	{
		PauseWidget->RemoveFromParent();
	}
	if (GameOverWidget && GameOverWidget->IsInViewport())
	{
		GameOverWidget->RemoveFromParent();
	}
	if (PerkWidget && PerkWidget->IsInViewport())
	{
		PerkWidget->RemoveFromParent();
	}
}

void ATGHUD::UpdateUIByState(ETGGameFlowState NewState)
{
	APlayerController* PC = GetOwningPlayerController();
	if (!PC)
	{
		return;
	}

	HideAllWidgets();

	if (NewState == ETGGameFlowState::Playing)
	{
		AddtoViewportPlayerWidget(PC);
	}
	else if (NewState == ETGGameFlowState::LevelUp)
	{
		AddtoViewportPerkWidget(PC);
	}
	else if (NewState == ETGGameFlowState::Paused)
	{
		AddtoViewportPausedWidget(PC);
	}
	else if (NewState == ETGGameFlowState::GameOver)
	{
		AddtoViewportGameOverWidget(PC);
	}

	OldState = NewState;
}

void ATGHUD::AddtoViewportPlayerWidget(APlayerController* PC)
{
	if (!PlayerWidget && PlayerWidgetClass)
	{
		PlayerWidget = CreateWidget<UTGPlayerWidget>(PC, PlayerWidgetClass);
	}

	if (PlayerWidget)
	{
		if (!PlayerWidget->IsInViewport())
		{
			PlayerWidget->AddToViewport();
		}
		PlayerWidget->SetVisibility(ESlateVisibility::Visible);
	}

	PC->bShowMouseCursor = false;
	PC->SetInputMode(FInputModeGameOnly());
}

void ATGHUD::AddtoViewportPerkWidget(APlayerController* PC)
{
	if (!PerkWidget && PerkWidgetClass)
	{
		PerkWidget = CreateWidget<UTGPerkSelectWidget>(PC, PerkWidgetClass);
	}

	if (PerkWidget && !PerkWidget->IsInViewport())
	{
		PerkWidget->AddToViewport();
	}

	PC->bShowMouseCursor = true;
	PC->SetInputMode(FInputModeGameAndUI());
}

void ATGHUD::AddtoViewportPausedWidget(APlayerController* PC)
{
	if (!PauseWidget && PauseWidgetClass)
	{
		PauseWidget = CreateWidget<UTGPauseWidget>(PC, PauseWidgetClass);
	}

	if (PauseWidget)
	{
		PauseWidget->AddToViewport();
	}

	PC->bShowMouseCursor = true;
	PC->SetInputMode(FInputModeGameAndUI());
}

void ATGHUD::AddtoViewportGameOverWidget(APlayerController* PC)
{
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
}
