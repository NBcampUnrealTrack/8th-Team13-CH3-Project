// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "TGInteractiveActor.h"
#include "Blueprint/UserWidget.h"
#include "TGNPCBase.generated.h"

UCLASS()
class TOWERGAME_API ATGNPCBase : public ATGInteractiveActor
{
	GENERATED_BODY()

public:
	ATGNPCBase();

protected:
	virtual void BeginPlay() override;

	// 플레이어가 상호작용 키 눌렀을 때 자동 호출
	virtual void OnInteract_Implementation(ATGPlayer* Player) override;

public:
	// NPC 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
	FString NPCName;

	// 대화 목록
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
	TArray<FText> DialogLines;

	// 현재 몇 번째 대화인지
	int32 CurrentDialogIndex;

	// 대화창 UI 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dialog")
	TSubclassOf<UUserWidget> DialogWidgetClass;

	// 현재 열려있는 대화창
	UPROPERTY()
	UUserWidget* DialogWidget;

	// 대화 시작
	UFUNCTION(BlueprintCallable, Category = "Dialog")
	void StartDialog();

	// 다음 대화
	UFUNCTION(BlueprintCallable, Category = "Dialog")
	void NextDialog();

	// 대화 끝났는지 확인
	UFUNCTION(BlueprintCallable, Category = "Dialog")
	bool IsDialogFinished();

	// 대화창 닫기
	UFUNCTION(BlueprintCallable, Category = "Dialog")
	void CloseDialog();

private:
	UFUNCTION()
	void OnNextButtonClicked();

	UFUNCTION()
	void OnSkipButtonClicked();
};
