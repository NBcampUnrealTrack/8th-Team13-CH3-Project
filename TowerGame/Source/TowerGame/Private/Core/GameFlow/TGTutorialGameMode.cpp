#include "Core/GameFlow/TGTutorialGameMode.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

ATGTutorialGameMode::ATGTutorialGameMode()
{
}

void ATGTutorialGameMode::BeginPlay()
{
	ResetTravelState();

	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("[TGTutorialGameMode] BeginPlay Complete"));
}

void ATGTutorialGameMode::StartGameFlow()
{
	ChangeFlowState(ETGGameFlowState::Playing);

	UE_LOG(LogTemp, Warning, TEXT("[TGTutorialGameMode] StartGameFlow - Wave Disabled"));
}

void ATGTutorialGameMode::ResetTravelState()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// 메뉴 / 이전 레벨에서 Pause가 걸려 있었을 가능성 제거
	UGameplayStatics::SetGamePaused(World, false);

	// GameOver 등에서 느려진 시간 복구
	if (World->GetWorldSettings())
	{
		World->GetWorldSettings()->SetTimeDilation(1.0f);
	}

	// 메인 메뉴 / 로딩 / 가이드 위젯 등이 남아서 입력을 먹는 문제 방지
	UWidgetLayoutLibrary::RemoveAllWidgets(World);

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TGTutorialGameMode] PlayerController is null"));
		return;
	}

	// 메뉴에서 UIOnly였던 입력 모드를 게임 입력으로 복구
	PC->SetInputMode(FInputModeGameOnly());
	PC->bShowMouseCursor = false;

	UE_LOG(LogTemp, Warning, TEXT("[TGTutorialGameMode] ResetTravelState Complete"));
}

void ATGTutorialGameMode::StartStage()
{
	// 스테이지 이동 직전에 다시 정리
	ResetTravelState();

	UE_LOG(LogTemp, Warning, TEXT("[TGTutorialGameMode] OpenLevel: %s"), *StageLevelName.ToString());

	UGameplayStatics::OpenLevel(this, StageLevelName);
}
