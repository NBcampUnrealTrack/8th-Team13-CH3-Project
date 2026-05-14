#include "BaseTower/TGBuildWidget.h"
#include "BaseTower/TGBaseTower.h"
#include "Components/TextBlock.h"
#include "Core/GameFlow/TGGameMode.h"
#include "Kismet/GameplayStatics.h"

void UTGBuildWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 슬롯 이름 초기값 세팅 — 이미지는 블루프린트에서 배치
	if (Text_Slot1Name) Text_Slot1Name->SetText(FText::FromString(TEXT("BASE")));
	if (Text_Slot2Name) Text_Slot2Name->SetText(FText::FromString(TEXT("TURRET")));
	if (Text_Slot3Name) Text_Slot3Name->SetText(FText::FromString(TEXT("DEBUFF")));
	if (Text_Slot4Name) Text_Slot4Name->SetText(FText::FromString(TEXT("SUPPORT")));
}

void UTGBuildWidget::SetOwnerTower(ABaseTower* InTower)
{
	// 이 위젯이 표시할 BaseTower 연결하고 UI 갱신
	OwnerTower = InTower;
	RefreshUI();
}

void UTGBuildWidget::RefreshUI()
{
	// 현재 보유 에너지 표시
	ATGGameMode* GM = Cast<ATGGameMode>(UGameplayStatics::GetGameMode(this));
	if (GM)
	{
		if (Text_Energy)
		{
			Text_Energy->SetText(FText::AsNumber(GM->GetCurrentEnergy()));
		}
	}
}
