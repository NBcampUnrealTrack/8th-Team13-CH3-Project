#pragma once
#include "CoreMinimal.h"
#include "TGGameFlowState.generated.h"

UENUM(BlueprintType)
enum class ETGGameFlowState : uint8
{
	Title UMETA(DisplayName = "Title"),
	MainMenu UMETA(DisplayName = "MainMenu"),
	Playing UMETA(DisplayName = "Playing"),
	BuildMode UMETA(DisplayName = "BuildMode"),
	Paused UMETA(DisplayName = "Paused"),
	WaveClear UMETA(DisplayName = "WaveClear"),
	GameOver UMETA(DisplayName = "GameOver"),
	Result UMETA(DisplayName = "Result")
};
