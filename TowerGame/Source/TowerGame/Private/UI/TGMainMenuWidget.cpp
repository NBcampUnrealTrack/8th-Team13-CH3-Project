#include "UI/TGMainMenuWidget.h"
#include "Components/Button.h"
#include "Components/Widget.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"
#include "Engine/Texture2D.h"
#include "Kismet/GameplayStatics.h"
#include "Core/GameFlow/TGMenuGameMode.h"

void UTGMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (StartButton)
	{
		StartButton->OnClicked.AddDynamic(this, &UTGMainMenuWidget::HandleStartClicked);
	}

	if (EndButton)
	{
		EndButton->OnClicked.AddDynamic(this, &UTGMainMenuWidget::HandleEndClicked);
	}

	if (GuideButton)
	{
		GuideButton->OnClicked.AddDynamic(this, &UTGMainMenuWidget::HandleGuideClicked);
	}

	//if (GuideClickButton)
	//{
	//	GuideClickButton->OnClicked.AddDynamic(this, &UTGMainMenuWidget::HandleGuidePageClicked);
	//}

	if (PrevGuideButton)
	{
		PrevGuideButton->OnClicked.AddDynamic(this, &UTGMainMenuWidget::HandlePrevGuideClicked);
	}

	if (NextGuideButton)
	{
		NextGuideButton->OnClicked.AddDynamic(this, &UTGMainMenuWidget::HandleNextGuideClicked);
	}

	if (ExitGuideButton)
	{
		ExitGuideButton->OnClicked.AddDynamic(this, &UTGMainMenuWidget::HandleExitGuideClicked);
	}

	CurrentGuideIndex = 0;

	RebuildGuideDots();
	UpdateGuideImage();
	UpdateGuideDots();

	HideGuide();
}

void UTGMainMenuWidget::HandleStartClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Start Button Clicked"));

	if (ATGMenuGameMode* MGM = Cast<ATGMenuGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		MGM->StartGame();
	}
}

void UTGMainMenuWidget::HandleEndClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("End Button Clicked"));

	if (ATGMenuGameMode* MGM = Cast<ATGMenuGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		MGM->EndGame();
	}
}

void UTGMainMenuWidget::HandleGuideClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Guide Button Clicked"));

	CurrentGuideIndex = 0;

	RebuildGuideDots();
	UpdateGuideImage();
	UpdateGuideDots();

	ShowGuide();
}

//void UTGMainMenuWidget::HandleGuidePageClicked()
//{
//	if (GuideTextures.Num() == 0)
//	{
//		HideGuide();
//		return;
//	}
//
//	// 마지막 페이지에서 클릭하면 메인메뉴로 복귀
//	if (CurrentGuideIndex >= GuideTextures.Num() - 1)
//	{
//		HideGuide();
//		return;
//	}
//
//	// 아직 마지막 페이지가 아니면 다음 페이지로 이동
//	CurrentGuideIndex++;
//
//	UpdateGuideImage();
//	UpdateGuideDots();
//}

void UTGMainMenuWidget::HandlePrevGuideClicked()
{
	if (GuideTextures.Num() == 0)
	{
		return;
	}

	if (CurrentGuideIndex <= 0)
	{
		return;
	}

	CurrentGuideIndex--;

	UpdateGuideImage();
	UpdateGuideDots();
	UpdateGuideButton();
}

void UTGMainMenuWidget::HandleNextGuideClicked()
{
	if (GuideTextures.Num() == 0)
	{
		return;
	}

	if (CurrentGuideIndex >= GuideTextures.Num() - 1)
	{
		return;
	}
	
	CurrentGuideIndex++;
	
	UpdateGuideImage();
	UpdateGuideDots();
	UpdateGuideButton();
}

void UTGMainMenuWidget::HandleExitGuideClicked()
{
	HideGuide();
}

void UTGMainMenuWidget::ShowGuide()
{
	if (MainMenuPanel)
	{
		MainMenuPanel->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (GuidePanel)
	{
		GuidePanel->SetVisibility(ESlateVisibility::Visible);
	}
}

void UTGMainMenuWidget::HideGuide()
{
	if (MainMenuPanel)
	{
		MainMenuPanel->SetVisibility(ESlateVisibility::Visible);
	}

	if (GuidePanel)
	{
		GuidePanel->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UTGMainMenuWidget::UpdateGuideImage()
{
	if (!GuideImage)
	{
		return;
	}

	if (!GuideTextures.IsValidIndex(CurrentGuideIndex))
	{
		return;
	}

	UTexture2D* CurrentTexture = GuideTextures[CurrentGuideIndex];

	if (!CurrentTexture)
	{
		return;
	}

	GuideImage->SetBrushFromTexture(CurrentTexture, false);
}

void UTGMainMenuWidget::RebuildGuideDots()
{
	if (!GuideDotPanel)
	{
		return;
	}

	GuideDotPanel->ClearChildren();
	GuideDotTexts.Empty();

	for (int32 i = 0; i < GuideTextures.Num(); i++)
	{
		UTextBlock* DotText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());

		if (!DotText)
		{
			continue;
		}

		DotText->SetText(FText::FromString(TEXT("●")));
		DotText->SetColorAndOpacity(FSlateColor(InactiveDotColor));
		DotText->SetRenderScale(FVector2D(1.5f, 1.5f));

		UHorizontalBoxSlot* DotSlot = GuideDotPanel->AddChildToHorizontalBox(DotText);

		if (DotSlot)
		{
			DotSlot->SetPadding(FMargin(6.0f, 0.0f));
			DotSlot->SetHorizontalAlignment(HAlign_Center);
			DotSlot->SetVerticalAlignment(VAlign_Center);
		}

		GuideDotTexts.Add(DotText);
	}
}

void UTGMainMenuWidget::UpdateGuideDots()
{
	for (int32 i = 0; i < GuideDotTexts.Num(); i++)
	{
		if (!GuideDotTexts[i])
		{
			continue;
		}

		if (i == CurrentGuideIndex)
		{
			GuideDotTexts[i]->SetColorAndOpacity(FSlateColor(ActiveDotColor));
		}
		else
		{
			GuideDotTexts[i]->SetColorAndOpacity(FSlateColor(InactiveDotColor));
		}
	}
}

void UTGMainMenuWidget::UpdateGuideButton()
{
	const bool bHasGuidePages = GuideTextures.Num() > 0;
	const bool bIsFirstPage = CurrentGuideIndex <= 0;
	const bool bIsLastPage = CurrentGuideIndex >= GuideTextures.Num() - 1;

	if (PrevGuideButton)
	{
		PrevGuideButton->SetIsEnabled(bHasGuidePages && !bIsFirstPage);
	}

	if (NextGuideButton)
	{
		NextGuideButton->SetIsEnabled(bHasGuidePages && !bIsLastPage);
	}
}
