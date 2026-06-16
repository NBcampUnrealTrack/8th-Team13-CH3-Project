#include "Core/GameFlow/TGGameMode.h"
#include "Enemies/TGCoreBase.h"
#include "Enemies/TGWaveManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/AssetManager.h"
#include "Player/TGPlayer.h"
#include "Perk/TGPerkComponent.h"

ATGGameMode::ATGGameMode()
	: SetWaveStartTime(10.0f)
{
	CurrentState = ETGGameFlowState::Title;

	PerkComponent = CreateDefaultSubobject<UTGPerkComponent>(TEXT("PerkComponent"));
}

void ATGGameMode::BeginPlay()
{
	Super::BeginPlay();

	// 진행도/토큰 런타임 초기화 (BP에서 오버라이드한 설정값 반영)
	CurrentLevel = 1;
	CurrentExp = 0;
	ExpToNextLevel = BaseExpToNextLevel;
	BuildTokens = StartingBuildTokens;

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
}

void ATGGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(WaveStartTimerHandle);
	GetWorldTimerManager().ClearTimer(GameOverTimerHandle);

	if (PreloadHandle.IsValid())
	{
		PreloadHandle->CancelHandle();
		PreloadHandle.Reset();
	}

	Super::EndPlay(EndPlayReason);
}

void ATGGameMode::ChangeFlowState(ETGGameFlowState NewState)
{
	if (CurrentState == NewState)
	{
		return;
	}

	OldState = CurrentState;

	// 이전 상태에서 빠져나갈 때 처리
	if (OldState == ETGGameFlowState::Paused || OldState == ETGGameFlowState::GameOver || OldState == ETGGameFlowState::LevelUp)
	{
		UGameplayStatics::SetGamePaused(GetWorld(), false);
	}

	CurrentState = NewState;

	// 새 상태로 들어갈 때 처리
	if (CurrentState == ETGGameFlowState::Paused || CurrentState == ETGGameFlowState::GameOver || CurrentState == ETGGameFlowState::LevelUp)
	{
		UGameplayStatics::SetGamePaused(GetWorld(), true);
	}

	UE_LOG(LogTemp, Log, TEXT("Flow State Changed: %s -> %s"),
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

void ATGGameMode::EnterLevelUp()
{
	// 플레이 또는 빌드모드에서만 진입. 이미 LevelUp이면 무시 (연속 레벨업 체이닝 허용).
	if (CurrentState == ETGGameFlowState::Playing || CurrentState == ETGGameFlowState::BuildMode)
	{
		ChangeFlowState(ETGGameFlowState::LevelUp);
	}
}

void ATGGameMode::ExitLevelUp()
{
	if (CurrentState == ETGGameFlowState::LevelUp)
	{
		ChangeFlowState(ETGGameFlowState::Playing);
	}
}

void ATGGameMode::PauseGameFlow()
{
	if (CurrentState == ETGGameFlowState::Playing || CurrentState == ETGGameFlowState::BuildMode)
	{
		if (ATGPlayer* Player = Cast<ATGPlayer>(UGameplayStatics::GetPlayerPawn(this, 0)))
		{
			Player->ClearFocusedEnemy();
		}

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

void ATGGameMode::HandleGameOver()
{
	const float PlayRate = 0.2f;
	GetWorldTimerManager().ClearTimer(WaveStartTimerHandle);
	GetWorldTimerManager().ClearTimer(GameOverTimerHandle);
	
	GetWorldSettings()->SetTimeDilation(PlayRate);
	OnHideWidgets.Broadcast();

	GetWorldTimerManager().SetTimer(
		GameOverTimerHandle,
		this,
		&ATGGameMode::FinishGameOver,
		0.5f,
		false
	);
}

void ATGGameMode::FinishGameOver()
{
	GetWorldTimerManager().ClearTimer(GameOverTimerHandle);
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

void ATGGameMode::BackMainMenu()
{
	UGameplayStatics::OpenLevel(this, FName(TEXT("L_MainMenu")));
}

void ATGGameMode::AddExp(int32 Amount)
{
	if (Amount <= 0) return;

	CurrentExp += Amount;

	// 누적 경험치가 요구치를 넘으면 (한 번에 여러 레벨 가능) 레벨업 반복
	while (ExpToNextLevel > 0 && CurrentExp >= ExpToNextLevel)
	{
		CurrentExp -= ExpToNextLevel;
		LevelUp();
	}

	OnExpChanged.Broadcast(CurrentExp, ExpToNextLevel, CurrentLevel);
}

void ATGGameMode::LevelUp()
{
	CurrentLevel++;
	ExpToNextLevel = BaseExpToNextLevel + (CurrentLevel - 1) * ExpPerLevelGrowth;

	// 레벨업 보상: 건설 토큰 지급 (특성 선택 시스템(#3) 도입 전 임시 기본 보상)
	if (BuildTokensPerLevel > 0)
	{
		AddBuildToken(BuildTokensPerLevel);
	}

	UE_LOG(LogTemp, Log, TEXT("[TGGameMode] LevelUp -> Lv.%d (NextExp %d)"), CurrentLevel, ExpToNextLevel);

	// 특성 선택 등 레벨업 후속 처리는 이 이벤트 구독자가 담당
	OnLevelUp.Broadcast(CurrentLevel);
}

bool ATGGameMode::TryConsumeBuildToken(int32 Amount)
{
	if (Amount <= 0) return false;
	if (BuildTokens < Amount) return false;

	BuildTokens -= Amount;
	OnBuildTokenChanged.Broadcast(BuildTokens);
	return true;
}

void ATGGameMode::AddBuildToken(int32 Amount)
{
	if (Amount <= 0) return;

	BuildTokens += Amount;
	OnBuildTokenChanged.Broadcast(BuildTokens);
}

TSubclassOf<ATGMountedTower> ATGGameMode::GetTowerSubclass(ETGTurretType type)
{
	if (TowerMap.Contains(type))
	{
		return TowerMap[type];
	}
	return nullptr;
}

void ATGGameMode::EndGame()
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, false);
	}
}
