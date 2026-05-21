#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TGMenuGameMode.generated.h"

class USoundBase;
class UAudioComponent;

UCLASS()
class TOWERGAME_API ATGMenuGameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	// 메인 메뉴 배경음 에셋
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BGM")
	USoundBase* MenuBGM;

	// 재생 중인 배경음 컴포넌트 (정지 시 사용)
	UPROPERTY()
	UAudioComponent* BGMComp;

public:
	UFUNCTION(BlueprintCallable)
	void StartGame();

	UFUNCTION(BlueprintCallable)
	void EndGame();
};
