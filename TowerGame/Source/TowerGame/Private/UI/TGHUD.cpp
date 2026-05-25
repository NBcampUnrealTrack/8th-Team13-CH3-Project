#include "UI/TGHUD.h"
#include "UI/TGMainMenuWidget.h"
#include "UI/TGGameOverWidget.h"
#include "UI/TGPlayerWidget.h"
#include "UI/TGPlayingWidget.h"
#include "UI/TGPauseWidget.h"
#include "BaseTower/TGBuildWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Core/GameFlow/TGGameMode.h"
#include "Core/Grid/TGGridBase.h"
#include "Enemies/TGWaveManager.h"
#include "Enemies/TGEnemyBase.h"
#include "Player/TGPlayer.h"

ATGHUD::ATGHUD()
	: CachedGameMode(nullptr),
	PlayerWidget(nullptr),
	PauseWidget(nullptr),
	GameOverWidget(nullptr),
	BuildWidget(nullptr),
	CachedWaveManager(nullptr)
{
}

void ATGHUD::BeginPlay()
{
	Super::BeginPlay();

	CachedGameMode = Cast<ATGGameMode>(UGameplayStatics::GetGameMode(this));
	if (CachedGameMode)
	{
		CachedGameMode->OnFlowStateChanged.AddDynamic(this, &ATGHUD::HandleFlowStateChanged);
		CachedGameMode->OnHideWidgets.AddDynamic(this, &ATGHUD::HideAllWidgets);

		UpdateUIByState(CachedGameMode->CurrentState);
	}

	if (ATGPlayer* Player = Cast<ATGPlayer>(GetOwningPawn()))
	{
		Player->OnTurretTypeSelected.AddDynamic(this, &ATGHUD::HandleTurretTypeSelected);
	}
}

void ATGHUD::HandleFlowStateChanged(ETGGameFlowState NewState)
{
	UE_LOG(LogTemp, Warning, TEXT("HUD State Changed: %s"), *UEnum::GetValueAsString(NewState));
	UpdateUIByState(NewState);
}

void ATGHUD::HideAllWidgets()
{
	if (PlayerWidget && PlayerWidget->IsInViewport())
	{
		PlayerWidget->RemoveFromParent();
	}
	if (PauseWidget && PauseWidget->IsInViewport())
	{
		PauseWidget->RemoveFromParent();
	}

	if (GameOverWidget && GameOverWidget->IsInViewport())
	{
		GameOverWidget->RemoveFromParent();
	}

	if (BuildWidget && BuildWidget->IsInViewport())
	{
		BuildWidget->RemoveFromParent();
	}
}

void ATGHUD::UpdateUIByState(ETGGameFlowState NewState)
{
	APlayerController* PC = GetOwningPlayerController();
	if (!PC)
	{
		return;
	}


	if (NewState == ETGGameFlowState::Playing && OldState == ETGGameFlowState::BuildMode)
	{
		UE_LOG(LogTemp, Warning, TEXT("BuildWidget Close"));
		if (BuildWidget && BuildWidget->IsInViewport())
		{
			BuildWidget->RemoveFromParent();
			UE_LOG(LogTemp, Warning, TEXT("BuildWidget Close2"));
		}
		OldState = NewState;
		return;
	}

	HideAllWidgets();

	if (NewState == ETGGameFlowState::BuildMode)
	{
		AddtoViewportBuildWidget(PC);
	}

	if (NewState == ETGGameFlowState::Playing || NewState == ETGGameFlowState::BuildMode)
	{
		AddtoViewportPlayerWidget(PC);
	}
	else if (NewState == ETGGameFlowState::Paused)
	{
		AddtoViewportPausedWidget(PC);
	}
	else if (NewState == ETGGameFlowState::GameOver)
	{
		AddtoViewportGameOverWidget(PC);
	}

	OldState = NewState;
}

void ATGHUD::AddtoViewportPlayerWidget(APlayerController* PC)
{
	if (!PlayerWidget && PlayerWidgetClass)
	{
		PlayerWidget = CreateWidget<UTGPlayerWidget>(PC, PlayerWidgetClass);
	}

	if (PlayerWidget)
	{
		PlayerWidget->AddToViewport();

		AActor* FoundActor = UGameplayStatics::GetActorOfClass(
			GetWorld(),
			ATGGridBase::StaticClass()
		);

		ATGGridBase* GridBase = Cast<ATGGridBase>(FoundActor);

		if (GridBase)
		{
			PlayerWidget->SetGridBase(GridBase);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("HUD: GridBase not found"));
		}

		BindWaveManager();
	}

	PC->bShowMouseCursor = false;
	PC->SetInputMode(FInputModeGameOnly());
}

void ATGHUD::AddtoViewportBuildWidget(APlayerController* PC)
{
	if (!BuildWidget && BuildWidgetClass)
	{
		BuildWidget = CreateWidget<UTGBuildWidget>(PC, BuildWidgetClass);
	}

	if (BuildWidget)
	{
		BuildWidget->AddToViewport();

		// 빌드모드 진입 시 현재 선택 타입으로 즉시 강조 갱신
		if (ATGPlayer* Player = Cast<ATGPlayer>(PC->GetPawn()))
		{
			BuildWidget->RefreshSlotHighlight(Player->GetSelectedTurretType());
		}
	}

	PC->bShowMouseCursor = false;
	PC->SetInputMode(FInputModeGameOnly());
}

void ATGHUD::HandleTurretTypeSelected(ETGTurretType SelectedType)
{
	if (BuildWidget)
	{
		BuildWidget->RefreshSlotHighlight(SelectedType);
	}
}

void ATGHUD::AddtoViewportPausedWidget(APlayerController* PC)
{
	if (!PauseWidget && PauseWidgetClass)
	{
		PauseWidget = CreateWidget<UTGPauseWidget>(PC, PauseWidgetClass);
	}

	if (PauseWidget)
	{
		PauseWidget->AddToViewport();
	}

	PC->bShowMouseCursor = true;
	PC->SetInputMode(FInputModeGameAndUI());
}

void ATGHUD::AddtoViewportGameOverWidget(APlayerController* PC)
{
	if (!GameOverWidget && GameOverWidgetClass)
	{
		GameOverWidget = CreateWidget<UTGGameOverWidget>(PC, GameOverWidgetClass);
	}

	if (GameOverWidget)
	{
		GameOverWidget->AddToViewport();
	}

	PC->bShowMouseCursor = true;
	PC->SetInputMode(FInputModeUIOnly());
}

void ATGHUD::BindWaveManager()
{
	if (CachedWaveManager)
	{
		return;
	}

	CachedWaveManager = ATGWaveManager::Get(this);

	if (!CachedWaveManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("HUD: WaveManager not found"));
		return;
	}

	CachedWaveManager->OnEnemySpawned.AddUniqueDynamic(
		this,
		&ATGHUD::HandleEnemySpawned
	);

	CachedWaveManager->OnEnemyRemovedForMiniMap.AddUniqueDynamic(
		this,
		&ATGHUD::HandleEnemyRemovedForMiniMap
	);

	UE_LOG(LogTemp, Warning, TEXT("HUD: WaveManager Bound"));
}

void ATGHUD::HandleEnemySpawned(ATGEnemyBase* SpawnedEnemy)
{
	if (!IsValid(SpawnedEnemy))
	{
		return;
	}

	if (!PlayerWidget)
	{
		return;
	}

	PlayerWidget->RegisterMonsterToMiniMap(SpawnedEnemy);
}

void ATGHUD::HandleEnemyRemovedForMiniMap(ATGEnemyBase* RemovedEnemy)
{
	if (!RemovedEnemy)
	{
		return;
	}

	if (!PlayerWidget)
	{
		return;
	}

	PlayerWidget->UnregisterMonsterFromMiniMap(RemovedEnemy);
}
