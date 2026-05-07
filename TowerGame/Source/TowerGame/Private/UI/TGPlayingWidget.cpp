#include "UI/TGPlayingWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Core/GameFlow/TGGameMode.h"


void UTGPlayingWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (PauseButton)
	{
		PauseButton->OnClicked.AddUniqueDynamic(this, &UTGPlayingWidget::HandlePauseClicked);
	}
}

void UTGPlayingWidget::HandlePauseClicked()
{
	if (ATGGameMode* GM = Cast<ATGGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->PauseGameFlow();
	}
}
