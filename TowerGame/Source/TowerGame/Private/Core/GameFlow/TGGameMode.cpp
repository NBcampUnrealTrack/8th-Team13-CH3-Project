#include "Core/GameFlow/TGGameMode.h"
#include "Enemies/TGCoreBase.h"
#include "Enemies/TGWaveManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/AssetManager.h"

ATGGameMode::ATGGameMode()
	: SetWaveStartTime(10.0f), CurrentEnergy(500)
{
	CurrentState = ETGGameFlowState::Title;
}

void ATGGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (GameBGM)
	{
		UGameplayStatics::SpawnSound2D(this, GameBGM);
	}

	// 에셋 사전 로드
	TArray<FSoftObjectPath> Paths;
	for (const TSoftObjectPtr<UObject>& Asset : PreloadAssets)
	{
		if (!Asset.IsNull())
		{
			Paths.Add(Asset.ToSoftObjectPath());
		}
	}

	if (!Paths.IsEmpty())
	{
		PreloadHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(
			Paths,
			FStreamableDelegate::CreateLambda([this]()
			{
				UE_LOG(LogTemp, Log, TEXT("[TGGameMode] 에셋 사전 로드 완료 (%d개)"), PreloadAssets.Num());
			})
		);
	}

	StartGameFlow();

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

	OldState = CurrentState;

	// 이전 상태에서 빠져나갈 때 처리
	if (OldState == ETGGameFlowState::Paused || OldState == ETGGameFlowState::GameOver)
	{
		UGameplayStatics::SetGamePaused(GetWorld(), false);
	}

	CurrentState = NewState;

	// 새 상태로 들어갈 때 처리
	if (CurrentState == ETGGameFlowState::Paused || CurrentState == ETGGameFlowState::GameOver)
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
		if (OldState == ETGGameFlowState::BuildMode)
			ChangeFlowState(ETGGameFlowState::BuildMode);
		else
			ChangeFlowState(ETGGameFlowState::Playing);
	}
}

//void ATGGameMode::HandleWaveClear()
//{
//	ChangeFlowState(ETGGameFlowState::WaveClear);
//}

void ATGGameMode::HandleGameOver()
{
	const float PlayRate = 0.2f;
	GetWorldTimerManager().ClearTimer(WaveStartTimerHandle);
	
	GetWorldSettings()->SetTimeDilation(PlayRate);
	OnHideWidgets.Broadcast();

	GetWorldTimerManager().SetTimer(
		GameOverTimerHandle,
		FTimerDelegate::CreateLambda([this]()
			{
				GetWorldTimerManager().ClearTimer(GameOverTimerHandle);
				ChangeFlowState(ETGGameFlowState::GameOver);
			}), 0.5f, false
	);
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

void ATGGameMode::BackMainMenu()
{
	UGameplayStatics::OpenLevel(this, FName(TEXT("L_MainMenu")));
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

int32 ATGGameMode::GetCurrentEnergy() const
{
	return CurrentEnergy;
}

TSubclassOf<ATGMountedTower> ATGGameMode::GetTowerSubclass(ETGTurretType type)
{
	if (TowerMap.Contains(type))
	{
		return TowerMap[type];
	}
	return nullptr;
}
