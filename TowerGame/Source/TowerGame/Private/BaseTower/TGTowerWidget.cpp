#include "BaseTower/TGTowerWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "TGMountedTower.h"

void UTGTowerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UpgradeButton)
	{
		UpgradeButton->OnClicked.AddDynamic(this, &UTGTowerWidget::HandleUpgradeClicked);
	}
}

void UTGTowerWidget::SetOwnerTower(ATGMountedTower* InTower)
{
	OwnerTower = InTower;
	RefreshUI();
}

void UTGTowerWidget::RefreshUI()
{
	if (!OwnerTower) return;

	if (LevelText)
	{
		// "Lv. 1" 형식으로 표시
		LevelText->SetText(FText::FromString(
			FString::Printf(TEXT("Lv. %d"), OwnerTower->GetUpgradeLevel())
		));
	}
}

void UTGTowerWidget::HandleUpgradeClicked()
{
	if (!OwnerTower) return;

	OwnerTower->Upgrade();
	RefreshUI();
}
