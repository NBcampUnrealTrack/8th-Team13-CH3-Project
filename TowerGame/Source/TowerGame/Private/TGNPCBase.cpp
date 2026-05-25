// Fill out your copyright notice in the Description page of Project Settings.


#include "TGNPCBase.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Player/TGPlayer.h"

ATGNPCBase::ATGNPCBase()
{
	PrimaryActorTick.bCanEverTick = false;
	CurrentDialogIndex = 0;
}

void ATGNPCBase::BeginPlay()
{
	Super::BeginPlay();
	CurrentDialogIndex = 0;
}

void ATGNPCBase::OnInteract_Implementation(ATGPlayer* Player)
{
	// 이미 대화창이 열려있으면 무시
	if (DialogWidget) return;
	StartDialog();
}

void ATGNPCBase::StartDialog()
{
	CurrentDialogIndex = 0;

	if (!DialogWidgetClass) return;

	// 대화창 생성 후 화면에 표시
	DialogWidget = CreateWidget<UUserWidget>(GetWorld(), DialogWidgetClass);
	if (!DialogWidget) return;
	DialogWidget->AddToViewport();

	// 다음 버튼 클릭 이벤트 바인딩
	UButton* NextBtn = Cast<UButton>(DialogWidget->GetWidgetFromName(TEXT("Btn_Next")));
	if (NextBtn)
	{
		NextBtn->OnClicked.AddDynamic(this, &ATGNPCBase::OnNextButtonClicked);
	}

	// 스킵 버튼 클릭 이벤트 바인딩
	UButton* SkipBtn = Cast<UButton>(DialogWidget->GetWidgetFromName(TEXT("Btn_Skip")));
	if (SkipBtn)
	{
		SkipBtn->OnClicked.AddDynamic(this, &ATGNPCBase::OnSkipButtonClicked);
	}

	// NPC 이름 표시
	UTextBlock* NameText = Cast<UTextBlock>(DialogWidget->GetWidgetFromName(TEXT("Txt_NPCName")));
	if (NameText) NameText->SetText(FText::FromString(NPCName));

	// 첫 번째 대화 내용 표시
	UTextBlock* ContentText = Cast<UTextBlock>(DialogWidget->GetWidgetFromName(TEXT("Txt_DialogContent")));
	if (ContentText && DialogLines.Num() > 0) ContentText->SetText(DialogLines[0]);
}

void ATGNPCBase::OnNextButtonClicked()
{
	NextDialog();
}

void ATGNPCBase::OnSkipButtonClicked()
{
	CloseDialog();
}

void ATGNPCBase::NextDialog()
{
	if (IsDialogFinished()) { CloseDialog(); return; }

	CurrentDialogIndex++;

	if (IsDialogFinished()) { CloseDialog(); return; }

	// 다음 대화 내용 업데이트
	UTextBlock* ContentText = Cast<UTextBlock>(DialogWidget->GetWidgetFromName(TEXT("Txt_DialogContent")));
	if (ContentText) ContentText->SetText(DialogLines[CurrentDialogIndex]);
}

bool ATGNPCBase::IsDialogFinished()
{
	return CurrentDialogIndex >= DialogLines.Num();
}

void ATGNPCBase::CloseDialog()
{
	if (DialogWidget)
	{
		DialogWidget->RemoveFromParent();
		DialogWidget = nullptr;
	}
}
