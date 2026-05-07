#include "BaseTower/TGTowerWidget.h"
#include "Components/TextBlock.h"
#include "TGMountedTower.h"

void UTGTowerWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UTGTowerWidget::SetOwnerTower(ATGMountedTower* InTower)
{
	// 이 위젯이 표시할 타워를 연결하고 UI 갱신
	OwnerTower = InTower;
	RefreshUI();
}

void UTGTowerWidget::RefreshUI()
{
	if (!OwnerTower) return;

	// 타워의 현재 업그레이드 레벨을 "Lv. 1" 형식으로 텍스트에 표시
	if (LevelText)
	{
		LevelText->SetText(FText::FromString(
			FString::Printf(TEXT("Lv. %d"), OwnerTower->GetUpgradeLevel())
		));
	}
}
