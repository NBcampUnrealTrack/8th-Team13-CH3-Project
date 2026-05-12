#include "Core/GameFlow/TGMenuGameMode.h"
#include "Kismet/GameplayStatics.h"

void ATGMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("MenuGameMode BeginPlay"));
}

void ATGMenuGameMode::StartGame()
{
	UGameplayStatics::OpenLevel(this, FName(TEXT("L_Stage01")));
}
