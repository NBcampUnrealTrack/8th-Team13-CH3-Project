#include "Core/GameFlow/TGGameMode.h"
#include "Enemies/TGCoreBase.h"
#include "Enemies/TGWaveManager.h"
#include "Kismet/GameplayStatics.h"

ATGGameMode::ATGGameMode()
	: SetWaveStartTime(10.0f), CurrentEnergy(500)
{
	CurrentState = ETGGameFlowState::Title;
}

void ATGGameMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("TGGameMode BeginPlay"));
	ChangeFlowState(ETGGameFlowState::MainMenu);

	//CachedCore = Cast<ATGCoreBase>(UGameplayStatics::GetActorOfClass(this, ATGCoreBase::StaticClass()));
	//if (CachedCore)
	//{
	//	CachedCore->OnCoreDestroyed.AddDynamic(this, &ATGGameMode::OnCoreDestroyedFromDelegate);
	//}
}

//void ATGGameMode::OnCoreDestroyedFromDelegate()
//{
//	UE_LOG(LogTemp, Warning, TEXT("GameMode: Core Destroyed Event Received"));
//	HandleGameOver();
//}

void ATGGameMode::ChangeFlowState(ETGGameFlowState NewState)
{
	if (CurrentState == NewState)
	{
		return;
	}

	const ETGGameFlowState OldState = CurrentState;

	// 이전 상태에서 빠져나갈 때 처리
	if (OldState == ETGGameFlowState::Paused)
	{
		UGameplayStatics::SetGamePaused(GetWorld(), false);
	}

	CurrentState = NewState;

	// 새 상태로 들어갈 때 처리
	if (CurrentState == ETGGameFlowState::Paused)
	{
		UGameplayStatics::SetGamePaused(GetWorld(), true);
	}

	UE_LOG(LogTemp, Warning, TEXT("Flow State Changed: %s -> %s"),
		*UEnum::GetValueAsString(OldState),
		*UEnum::GetValueAsString(CurrentState));

	OnFlowStateChanged.Broadcast(CurrentState);
}

void ATGGameMode::StartGameFlow()
{
	ChangeFlowState(ETGGameFlowState::Playing);

	GetWorldTimerManager().SetTimer(
		WaveStartTimerHandle,
		this,
		&ATGGameMode::StartWave,
		SetWaveStartTime,
		false
	);
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
	GetWorldTimerManager().ClearTimer(WaveStartTimerHandle);
	ChangeFlowState(ETGGameFlowState::GameOver);
}

void ATGGameMode::HandleGameClear()
{
	ChangeFlowState(ETGGameFlowState::Result);
}

void ATGGameMode::StartWave()
{
	ATGWaveManager* WaveManager = ATGWaveManager::Get(this);
	if (!WaveManager)
	{
		UE_LOG(LogTemp, Error, TEXT("WaveManager is nullptr"));
		return;
	}

	WaveManager->StartNextWave();
}

bool ATGGameMode::IsPauseGameFlow()
{
	return CurrentState == ETGGameFlowState::Paused;
}

bool ATGGameMode::SpendEnergy(int32 Amount)
{
	if (CurrentEnergy < Amount) return false;
	CurrentEnergy -= Amount;
	OnEnergyChanged.Broadcast(CurrentEnergy);
	return true;
}

void ATGGameMode::AddEnergy(int32 Amount)
{
	CurrentEnergy += Amount;
	OnEnergyChanged.Broadcast(CurrentEnergy);
}

int32 ATGGameMode::GetEnergy() const
{
	return CurrentEnergy;
}
