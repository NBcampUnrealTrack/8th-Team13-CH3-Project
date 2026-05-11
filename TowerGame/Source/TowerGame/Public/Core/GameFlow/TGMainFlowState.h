#pragma once

#include "CoreMinimal.h"
#include "TGMainFlowState.generated.h"

UENUM(BlueprintType)
enum class ETGMainFlowState : uint8
{
	Title		UMETA(DisplayName = "Title"),
	MainMenu	UMETA(DisplayName = "MainMenu"),
	InGame		UMETA(DisplayName = "InGame"),
	Result		UMETA(DisplayName = "Result")
};
