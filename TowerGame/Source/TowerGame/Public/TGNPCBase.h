// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "TGInteractiveActor.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
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

	// 플레이어 시선이 닿을 때 아웃라인 표시
	virtual void OnFocused_Implementation(ATGPlayer* Player) override;

	// 플레이어 시선이 벗어날 때 아웃라인 제거
	virtual void OnUnfocused_Implementation(ATGPlayer* Player) override;
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
	// BP에서 추가한 Widget Component 참조
	UPROPERTY(BlueprintReadWrite, Category = "Dialog")
	UWidgetComponent* DialogWidgetComponent;
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
