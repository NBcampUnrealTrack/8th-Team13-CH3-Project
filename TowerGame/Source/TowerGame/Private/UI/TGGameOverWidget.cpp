#include "UI/TGGameOverWidget.h"
#include "Components/Button.h"
#include "Components/Widget.h"
#include "Kismet/GameplayStatics.h"
#include "Core/GameFlow/TGGameMode.h"

void UTGGameOverWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ResultButton)
	{
		ResultButton->OnClicked.AddDynamic(this, &UTGGameOverWidget::HandleResultClicked);
	}
	if (MainMenuButton)
	{
		MainMenuButton->OnClicked.AddDynamic(this, &UTGGameOverWidget::HandleMainMenuClicked);
	}

	HideResult();
	PlayGameOverAnimation();
}

void UTGGameOverWidget::HandleResultClicked()
{
	ShowResult();
}

void UTGGameOverWidget::HandleMainMenuClicked()
{
	if (ATGGameMode* GM = Cast<ATGGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->BackMainMenu();
	}
}

void UTGGameOverWidget::ShowResult()
{
	if (GameOverPanel)
	{
		GameOverPanel->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (ResultPanel)
	{
		ResultPanel->SetVisibility(ESlateVisibility::Visible);
	}
}

void UTGGameOverWidget::HideResult()
{
	if (GameOverPanel)
	{
		GameOverPanel->SetVisibility(ESlateVisibility::Visible);
	}

	if (ResultPanel)
	{
		ResultPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UTGGameOverWidget::PlayGameOverAnimation()
{
	if (FadeInDark)
	{
		PlayAnimation(FadeInDark);
	}
}
