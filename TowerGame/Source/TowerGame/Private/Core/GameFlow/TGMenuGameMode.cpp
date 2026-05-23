#include "Core/GameFlow/TGMenuGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/PlayerController.h"
#include "Components/AudioComponent.h"

void ATGMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("MenuGameMode BeginPlay"));

	if (MenuBGM)
	{
		BGMComp = UGameplayStatics::SpawnSound2D(this, MenuBGM);
	}
}

void ATGMenuGameMode::StartGame(FString OpenLevelPath)
{
	if (BGMComp)
	{
		BGMComp->Stop();
	}

	UGameplayStatics::OpenLevel(this, FName(*OpenLevelPath));
}

void ATGMenuGameMode::EndGame()
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, false);
	}
}
