// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GameFlow/TGGamePlayState.h"

#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Player/TGPlayerController.h"

void ATGGamePlayState::UpdateHUD()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ATGPlayerController* TGPlayerController = Cast<ATGPlayerController>(PlayerController))
		{
			if (UUserWidget* HUDWidget = TGPlayerController->GetHUDWidget())
			{
				if (UTextBlock* WaveText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName("WaveText")))
				{
					//	TODO : 웨이브 정보 출력
					WaveText->SetText(FText::FromString(FString::Printf(TEXT("Wave"))));
				}

				//	TODO : 데이터 <-> UI 연동 작업
			}
		}
	}
}
