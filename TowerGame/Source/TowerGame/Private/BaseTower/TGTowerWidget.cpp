#include "BaseTower/TGTowerWidget.h"
#include "BaseTower/BuffTower.h"
#include "BaseTower/DebuffTower.h"
#include "Components/TextBlock.h"
#include "TGMountedTower.h"
#include "TGWeaponTower.h"
#include "Core/GameFlow/TGGameMode.h"
#include "Kismet/GameplayStatics.h"

// 업그레이드에 필요한 건설 토큰 수
#define UPGRADE_TOKEN_COST 1

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

	// 현재 보유 건설 토큰 표시
	ATGGameMode* GM = Cast<ATGGameMode>(UGameplayStatics::GetGameMode(this));
	if (GM)
	{
		// 현재 보유 건설 토큰 표시
		if (BuildTokenText)
		{
			BuildTokenText->SetText(FText::FromString(
				FString::Printf(TEXT("Tokens: %d"), GM->GetBuildTokens())
			));
		}

		// 업그레이드 비용(토큰) 표시
		if (UpgradeCostText)
		{
			UpgradeCostText->SetText(FText::FromString(
				FString::Printf(TEXT("Cost: %d Token"), UPGRADE_TOKEN_COST)
			));
		}
	}

	// 웨폰타워인 경우 데미지, 사거리 표시
	ATGWeaponTower* WeaponTower = Cast<ATGWeaponTower>(OwnerTower);
	if (WeaponTower)
	{
		if (DamageText)
			DamageText->SetText(FText::FromString(
				FString::Printf(TEXT("Damage: %.1f"), WeaponTower->GetAttackDamage())
			));
		if (RangeText)
			RangeText->SetText(FText::FromString(
				FString::Printf(TEXT("Range: %.1f"), WeaponTower->GetAttackRange())
			));
	}
	// 버프타워인 경우
	ATGBuffTower* BuffTower = Cast<ATGBuffTower>(OwnerTower);
	if (BuffTower)
	{
		if (DamageText)
			DamageText->SetText(FText::FromString(
				FString::Printf(TEXT("Heal: %d"), BuffTower->GetHealAmount())
			));
		if (RangeText)
			RangeText->SetText(FText::FromString(
				FString::Printf(TEXT("Range: %.1f"), BuffTower->GetBuffRange())
			));
	}
	// 디버프타워인 경우
	ATGDebuffTower* DebuffTower = Cast<ATGDebuffTower>(OwnerTower);
	if (DebuffTower)
	{
		if (DamageText)
			DamageText->SetText(FText::FromString(
				FString::Printf(TEXT("Slow: %.0f%%"), (1.f - DebuffTower->GetSlowRate()) * 100.f)
			));
		if (RangeText)
			RangeText->SetText(FText::FromString(
				FString::Printf(TEXT("Range: %.1f"), DebuffTower->GetDebuffRange())
			));
	}
}
