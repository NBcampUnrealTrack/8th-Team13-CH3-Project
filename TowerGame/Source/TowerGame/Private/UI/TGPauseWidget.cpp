#include "UI/TGPauseWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Core/GameFlow/TGGameMode.h"

void UTGPauseWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ResumeButton)
	{
		ResumeButton->OnClicked.AddUniqueDynamic(this, &UTGPauseWidget::HandleResumeClicked);
	}
	if (MainMenuButton)
	{
		MainMenuButton->OnClicked.AddUniqueDynamic(this, &UTGPauseWidget::HandleMainMenuButtonClicked);
	}
	if (QuitGameButton)
	{
		QuitGameButton->OnClicked.AddUniqueDynamic(this, &UTGPauseWidget::HandleQuitGameClicked);
	}
}

void UTGPauseWidget::HandleResumeClicked()
{
	if (ATGGameMode* GM = Cast<ATGGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->ResumeGameFlow();
	}
}

void UTGPauseWidget::HandleMainMenuButtonClicked()
{
	if (ATGGameMode* GM = Cast<ATGGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->BackMainMenu();
	}
}

void UTGPauseWidget::HandleQuitGameClicked()
{
	if (ATGGameMode* GM = Cast<ATGGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->EndGame();
	}
}
