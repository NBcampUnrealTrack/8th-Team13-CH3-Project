#include "BaseTower/TGTowerWidget.h"
#include "Components/TextBlock.h"
#include "TGMountedTower.h"
#include "Core/GameFlow/TGGameMode.h"
#include "Kismet/GameplayStatics.h"

// 업그레이드 비용
#define UPGRADE_COST 20

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

	// 타워의 현재 업그레이드 레벨을 "Lv. 1" 형식으로 표시
	if (LevelText)
	{
		LevelText->SetText(FText::FromString(
			FString::Printf(TEXT("Lv. %d"), OwnerTower->GetUpgradeLevel())
		));
	}

	// 현재 자원량 표시
	ATGGameMode* GM = Cast<ATGGameMode>(UGameplayStatics::GetGameMode(this));
	if (GM)
	{
		if (EnergyText)
		{
			EnergyText->SetText(FText::FromString(
				FString::Printf(TEXT("자원: %d"), GM->GetCurrentEnergy())
			));
		}

		// 업그레이드 비용 표시
		if (UpgradeCostText)
		{
			UpgradeCostText->SetText(FText::FromString(
				FString::Printf(TEXT("업그레이드 비용: %d"), UPGRADE_COST)
			));
		}
	}
}
