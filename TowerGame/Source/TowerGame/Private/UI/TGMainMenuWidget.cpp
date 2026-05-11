#include "UI/TGMainMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Core/GameFlow/TGMenuGameMode.h"

void UTGMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (StartButton)
	{
		StartButton->OnClicked.AddDynamic(this, &UTGMainMenuWidget::HandleStartClicked);
	}
}

void UTGMainMenuWidget::HandleStartClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Start Button Clicked"));

	if (ATGMenuGameMode* MGM = Cast<ATGMenuGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		MGM->StartGame();
	}
}
