#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TGMenuGameMode.generated.h"

UCLASS()
class TOWERGAME_API ATGMenuGameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	void StartGame();
};
