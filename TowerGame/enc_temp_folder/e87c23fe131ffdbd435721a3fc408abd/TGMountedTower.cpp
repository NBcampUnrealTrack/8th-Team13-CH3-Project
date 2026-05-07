#include "TGMountedTower.h"
#include "Components/StaticMeshComponent.h"
#include "BaseTower/TGTowerWidget.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

ATGMountedTower::ATGMountedTower()
{
	PrimaryActorTick.bCanEverTick = false;

	// 무기 메시
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;
}

void ATGMountedTower::Upgrade()
{
	UpgradeLevel++;
}

// 플레이어 시선이 닿을 때 — 강조 표시
void ATGMountedTower::OnFocused_Implementation(ATGPlayer* Player)
{
	if (DynamicMaterial)
	{
		DynamicMaterial->SetScalarParameterValue(TEXT("Highlight"), 1.0f);
	}

	// 위젯 생성 및 표시
	if (TowerWidgetClass)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		if (PC)
		{
			if (!TowerWidget)
			{
				TowerWidget = CreateWidget<UTGTowerWidget>(PC, TowerWidgetClass);
			}
			TowerWidget->SetOwnerTower(this);
			TowerWidget->AddToViewport();
		}
	}
}

// 플레이어 시선이 벗어날 때 — 강조 해제
void ATGMountedTower::OnUnfocused_Implementation(ATGPlayer* Player)
{
	if (DynamicMaterial)
	{
		DynamicMaterial->SetScalarParameterValue(TEXT("Highlight"), 0.0f);
	}

	// 위젯 숨기기
	if (TowerWidget && TowerWidget->IsInViewport())
	{
		TowerWidget->RemoveFromParent();
	}
}

// 플레이어가 상호작용 키를 눌렀을 때
void ATGMountedTower::OnInteract_Implementation(ATGPlayer* Player)
{

}
