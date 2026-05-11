#include "UI/TGMenuHUD.h"
#include "Player/TGPlayerController.h"
#include "UI/TGMainMenuWidget.h"

ATGMenuHUD::ATGMenuHUD() :
	MainMenuWidget(nullptr)
{
}

void ATGMenuHUD::BeginPlay()
{
	Super::BeginPlay();
	ATGPlayerController* TGPC = Cast<ATGPlayerController>(GetOwningPlayerController());
	if (!TGPC)
	{
		return;
	}

	if (!MainMenuWidget && MainMenuWidgetClass)
	{
		MainMenuWidget = CreateWidget<UTGMainMenuWidget>(TGPC, MainMenuWidgetClass);
	}

	if (MainMenuWidget)
	{
		MainMenuWidget->AddToViewport();
	}

	TGPC->bShowMouseCursor = true;
	TGPC->SetInputMode(FInputModeUIOnly());
}
