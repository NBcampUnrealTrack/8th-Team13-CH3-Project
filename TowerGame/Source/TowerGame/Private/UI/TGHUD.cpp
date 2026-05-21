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

		UpdateUIByState(CachedGameMode->CurrentState);
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

	if (NewState == ETGGameFlowState::BuildMode && OldState == ETGGameFlowState::Playing)
	{
		AddtoViewportBuildWidget(PC);
		OldState = NewState;
		return;
	}

	HideAllWidgets();

	if (NewState == ETGGameFlowState::Playing)
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
	}

	PC->bShowMouseCursor = false;
	PC->SetInputMode(FInputModeGameOnly());
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
