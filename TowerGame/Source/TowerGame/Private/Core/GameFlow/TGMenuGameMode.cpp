#include "Core/GameFlow/TGMenuGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/PlayerController.h"

void ATGMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("MenuGameMode BeginPlay"));
}

void ATGMenuGameMode::StartGame()
{
	UGameplayStatics::OpenLevel(this, FName(TEXT("L_Stage01")));
}

void ATGMenuGameMode::EndGame()
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, false);
	}
}
