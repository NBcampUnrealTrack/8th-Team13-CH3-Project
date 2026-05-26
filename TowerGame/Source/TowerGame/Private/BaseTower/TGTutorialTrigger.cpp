// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseTower/TGTutorialTrigger.h"
#include "Player/TGPlayer.h"
#include "Components/BoxComponent.h"
#include "Blueprint/UserWidget.h"

ATGTutorialTrigger::ATGTutorialTrigger()
{
	PrimaryActorTick.bCanEverTick = false;
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	SetRootComponent(TriggerBox);
	TriggerBox->SetBoxExtent(FVector(100.f, 100.f, 100.f));
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ATGTutorialTrigger::BeginPlay()
{
	Super::BeginPlay();
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ATGTutorialTrigger::OnOverlapBegin);
}

void ATGTutorialTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	// 플레이어가 아니면 무시
	if (!Cast<ATGPlayer>(OtherActor)) return;
	// 한번만 트리거
	if (bTriggerOnce && bAlreadyTriggered) return;
	bAlreadyTriggered = true;
	if (!TutorialWidgetClass) return;
	// 위젯 생성 및 표시
	TutorialWidget = CreateWidget<UUserWidget>(GetWorld(), TutorialWidgetClass);
	if (!TutorialWidget) return;
	TutorialWidget->AddToViewport();
	// DisplayTime 후 위젯 제거
	GetWorldTimerManager().SetTimer(HideTimerHandle, this, &ATGTutorialTrigger::RemoveWidget, DisplayTime, false);
}

void ATGTutorialTrigger::RemoveWidget()
{
	if (TutorialWidget)
	{
		TutorialWidget->RemoveFromParent();
		TutorialWidget = nullptr;
	}
}
