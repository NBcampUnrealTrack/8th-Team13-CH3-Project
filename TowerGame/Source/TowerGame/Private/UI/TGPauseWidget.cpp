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
}

void UTGPauseWidget::HandleResumeClicked()
{
	if (ATGGameMode* GM = Cast<ATGGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->ResumeGameFlow();
	}
}
