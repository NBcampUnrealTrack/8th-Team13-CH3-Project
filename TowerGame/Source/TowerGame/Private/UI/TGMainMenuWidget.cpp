#include "UI/TGMainMenuWidget.h"
#include "Components/Button.h"
#include "Components/Widget.h"
#include "Kismet/GameplayStatics.h"
#include "Core/GameFlow/TGMenuGameMode.h"

void UTGMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (StartButton)
	{
		StartButton->OnClicked.AddDynamic(this, &UTGMainMenuWidget::HandleStartClicked);
	}

	if (EndButton)
	{
		EndButton->OnClicked.AddDynamic(this, &UTGMainMenuWidget::HandleEndClicked);
	}

	if (GuideButton)
	{
		GuideButton->OnClicked.AddDynamic(this, &UTGMainMenuWidget::HandleGuideClicked);
	}

	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &UTGMainMenuWidget::HandleBackClicked);
	}

	HideGuide();
}

void UTGMainMenuWidget::HandleStartClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Start Button Clicked"));

	if (ATGMenuGameMode* MGM = Cast<ATGMenuGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		MGM->StartGame();
	}
}

void UTGMainMenuWidget::HandleEndClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("End Button Clicked"));

	if (ATGMenuGameMode* MGM = Cast<ATGMenuGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		MGM->EndGame();
	}
}

void UTGMainMenuWidget::HandleGuideClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Guide Button Clicked"));
	ShowGuide();
}

void UTGMainMenuWidget::HandleBackClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Back Button Clicked"));
	HideGuide();
}

void UTGMainMenuWidget::ShowGuide()
{
	if (MainMenuPanel)
	{
		MainMenuPanel->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (GuidePanel)
	{
		GuidePanel->SetVisibility(ESlateVisibility::Visible);
	}
}

void UTGMainMenuWidget::HideGuide()
{
	if (MainMenuPanel)
	{
		MainMenuPanel->SetVisibility(ESlateVisibility::Visible);
	}

	if (GuidePanel)
	{
		GuidePanel->SetVisibility(ESlateVisibility::Collapsed);
	}
}
