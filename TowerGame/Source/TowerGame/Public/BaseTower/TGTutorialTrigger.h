// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Blueprint/UserWidget.h"
#include "TGTutorialTrigger.generated.h"

UCLASS()
class TOWERGAME_API ATGTutorialTrigger : public AActor
{
	GENERATED_BODY()
public:

	ATGTutorialTrigger();

protected:

	virtual void BeginPlay() override;
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
public:
	// 트리거 콜리전
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tutorial")
	UBoxComponent* TriggerBox;

	// 표시할 UI 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial")
	TSubclassOf<UUserWidget> TutorialWidgetClass;

	// UI가 표시될 시간 (초)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial")
	float DisplayTime = 2.0f;

	// 한번만 트리거되게 할지 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial")
	bool bTriggerOnce = true;

private:
	UPROPERTY()
	UUserWidget* TutorialWidget;
	FTimerHandle HideTimerHandle;
	bool bAlreadyTriggered = false;
	void RemoveWidget();
};
