#pragma once

#include "CoreMinimal.h"
#include "Core/GameFlow/TGGameMode.h"
#include "TGTutorialGameMode.generated.h"

UCLASS()
class TOWERGAME_API ATGTutorialGameMode : public ATGGameMode
{
	GENERATED_BODY()

public:
	ATGTutorialGameMode();

protected:
	virtual void BeginPlay() override;

public:
	// 기존 TGGameMode의 StartGameFlow에서 웨이브 시작만 제거한 버전
	virtual void StartGameFlow() override;

	// 메인 메뉴에서 넘어오면서 남은 UI / 입력 상태 정리
	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	void ResetTravelState();

	// 튜토리얼 끝나고 실제 스테이지로 이동할 때 사용
	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	void StartStage();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tutorial|Level")
	FName StageLevelName = TEXT("L_Stage01");
};
