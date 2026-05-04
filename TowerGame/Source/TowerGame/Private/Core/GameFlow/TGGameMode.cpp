#include "Core/GameFlow/TGGameMode.h"
#include "Kismet/GameplayStatics.h"

ATGGameMode::ATGGameMode()
{
	CurrentState = ETGGameFlowState::Title;
}

void ATGGameMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("TGGameMode BeginPlay"));
	ChangeFlowState(ETGGameFlowState::Title);
}

void ATGGameMode::ChangeFlowState(ETGGameFlowState NewState)
{
	if (CurrentState == NewState)
	{
		return;
	}

	CurrentState = NewState;

	switch (CurrentState)
	{
	case ETGGameFlowState::Paused:
		UGameplayStatics::SetGamePaused(GetWorld(), true);
		break;

	default:
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		break;
	}

	UE_LOG(LogTemp, Warning, TEXT("Flow State Changed: %s"),
		*UEnum::GetValueAsString(CurrentState));
}

void ATGGameMode::StartGameFlow()
{
	ChangeFlowState(ETGGameFlowState::Playing);
}

void ATGGameMode::EnterBuildMode()
{
	if (CurrentState == ETGGameFlowState::Playing)
	{
		ChangeFlowState(ETGGameFlowState::BuildMode);
	}
}

void ATGGameMode::ExitBuildMode()
{
	if (CurrentState == ETGGameFlowState::BuildMode)
	{
		ChangeFlowState(ETGGameFlowState::Playing);
	}
}

void ATGGameMode::PauseGameFlow()
{
	if (CurrentState == ETGGameFlowState::Playing || CurrentState == ETGGameFlowState::BuildMode)
	{
		ChangeFlowState(ETGGameFlowState::Paused);
	}
}

void ATGGameMode::ResumeGameFlow()
{
	if (CurrentState == ETGGameFlowState::Paused)
	{
		ChangeFlowState(ETGGameFlowState::Playing);
	}
}

void ATGGameMode::HandleWaveClear()
{
	ChangeFlowState(ETGGameFlowState::WaveClear);
}

void ATGGameMode::HandleGameOver()
{
	ChangeFlowState(ETGGameFlowState::GameOver);
}

void ATGGameMode::HandleGameClear()
{
	ChangeFlowState(ETGGameFlowState::Result);
}
