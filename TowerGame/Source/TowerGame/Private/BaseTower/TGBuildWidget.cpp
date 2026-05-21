#include "BaseTower/TGBuildWidget.h"
#include "Components/Image.h"

void UTGBuildWidget::NativeConstruct()
{
	Super::NativeConstruct();
	RefreshSlotHighlight(ETGTurretType::None);
}

void UTGBuildWidget::RefreshSlotHighlight(ETGTurretType SelectedType)
{
	auto ApplyColor = [&](UImage* SlotImage, bool bSelected)
	{
		if (SlotImage)
		{
			SlotImage->SetColorAndOpacity(bSelected ? SelectedColor : NormalColor);
		}
	};

	// 슬롯 순서: 1=BaseTower, 2=WeaponTower, 3=DebuffTower, 4=BuffTower
	ApplyColor(Image_Slot1, false);
	ApplyColor(Image_Slot2, SelectedType == ETGTurretType::WeaponTower);
	ApplyColor(Image_Slot3, SelectedType == ETGTurretType::DebuffTower);
	ApplyColor(Image_Slot4, SelectedType == ETGTurretType::BuffTower);
}
