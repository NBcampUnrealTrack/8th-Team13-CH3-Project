// Fill out your copyright notice in the Description page of Project Settings.


#include "TGNPCBase.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Player/TGPlayer.h"

ATGNPCBase::ATGNPCBase()
{
	PrimaryActorTick.bCanEverTick = false;
	CurrentDialogIndex = 0;
	DialogWidgetComponent = nullptr;

	// 상호작용 범위 콜리전 생성
	InteractRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractRangeSphere"));
	InteractRangeSphere->SetupAttachment(RootComponent);
	InteractRangeSphere->SetSphereRadius(300.f);
	InteractRangeSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractRangeSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractRangeSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ATGNPCBase::BeginPlay()
{
	Super::BeginPlay();
	CurrentDialogIndex = 0;
	// BP에서 추가한 Widget Component 찾기
	DialogWidgetComponent = FindComponentByClass<UWidgetComponent>();

	// 범위 콜리전 이벤트 바인딩
	InteractRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &ATGNPCBase::OnPlayerEnterRange);
	InteractRangeSphere->OnComponentEndOverlap.AddDynamic(this, &ATGNPCBase::OnPlayerExitRange);
}

void ATGNPCBase::OnPlayerEnterRange(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ATGPlayer>(OtherActor))
		bPlayerInRange = true;
}

void ATGNPCBase::OnPlayerExitRange(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<ATGPlayer>(OtherActor))
	{
		bPlayerInRange = false;
		CloseDialog();
	}
}

void ATGNPCBase::OnInteract_Implementation(ATGPlayer* Player)
{
	// 범위 밖이면 무시
	if (!bPlayerInRange) return;

	// 대화창이 열려있으면 다음 대화로 넘어감
	if (DialogWidget)
	{
		NextDialog();
		return;
	}
	StartDialog();
}

void ATGNPCBase::OnFocused_Implementation(ATGPlayer* Player)
{
	Super::OnFocused_Implementation(Player);
	// 스켈레탈 메시에 아웃라인 적용
	TArray<USkeletalMeshComponent*> MeshComps;
	GetComponents<USkeletalMeshComponent>(MeshComps);
	for (USkeletalMeshComponent* Mesh : MeshComps)
	{
		if (IsValid(Mesh))
		{
			Mesh->SetRenderCustomDepth(true);
			Mesh->SetCustomDepthStencilValue(1);
		}
	}
}

void ATGNPCBase::OnUnfocused_Implementation(ATGPlayer* Player)
{
	Super::OnUnfocused_Implementation(Player);
	// 아웃라인 제거
	TArray<USkeletalMeshComponent*> MeshComps;
	GetComponents<USkeletalMeshComponent>(MeshComps);
	for (USkeletalMeshComponent* Mesh : MeshComps)
	{
		if (IsValid(Mesh))
			Mesh->SetRenderCustomDepth(false);
	}
}

// FText에서 \n을 실제 줄바꿈으로 변환
static FText ConvertNewLines(const FText& InText)
{
	FString Str = InText.ToString();
	Str = Str.Replace(TEXT("\\n"), TEXT("\n"));
	return FText::FromString(Str);
}

void ATGNPCBase::StartDialog()
{
	CurrentDialogIndex = 0;
	if (DialogWidgetComponent)
	{
		// Widget Component 방식
		DialogWidgetComponent->SetVisibility(true);
		DialogWidget = Cast<UUserWidget>(DialogWidgetComponent->GetUserWidgetObject());
	}
	else
	{
		// 기존 AddToViewport 방식 (fallback)
		if (!DialogWidgetClass) return;
		DialogWidget = CreateWidget<UUserWidget>(GetWorld(), DialogWidgetClass);
		if (!DialogWidget) return;
		DialogWidget->AddToViewport();
	}
	if (!DialogWidget) return;
	// 입력 모드를 Game으로 유지 (E키 계속 작동하게)
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
	}
	// NPC 이름 표시
	UTextBlock* NameText = Cast<UTextBlock>(DialogWidget->GetWidgetFromName(TEXT("Txt_NPCName")));
	if (NameText) NameText->SetText(FText::FromString(NPCName));
	// 첫 번째 대화 내용 표시
	UTextBlock* ContentText = Cast<UTextBlock>(DialogWidget->GetWidgetFromName(TEXT("Txt_DialogContent")));
	if (ContentText && DialogLines.Num() > 0) ContentText->SetText(ConvertNewLines(DialogLines[0]));
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
	if (ContentText) ContentText->SetText(ConvertNewLines(DialogLines[CurrentDialogIndex]));
}

bool ATGNPCBase::IsDialogFinished()
{
	return CurrentDialogIndex >= DialogLines.Num();
}

void ATGNPCBase::CloseDialog()
{
	if (DialogWidgetComponent)
	{
		// Widget Component 숨기기
		DialogWidgetComponent->SetVisibility(false);
	}
	else if (DialogWidget)
	{
		DialogWidget->RemoveFromParent();
	}
	// DialogWidget은 nullptr로 초기화하지 않음 (Widget Component 방식에서 OnInteract 판단에 사용)
	if (!DialogWidgetComponent)
		DialogWidget = nullptr;
}
