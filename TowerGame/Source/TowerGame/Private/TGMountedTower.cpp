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
	WeaponMesh->SetupAttachment(RootComponent);

	// 외곽선 강조를 위한 커스텀 뎁스 스텐실 값 설정
	WeaponMesh->SetCustomDepthStencilValue(1);
}

void ATGMountedTower::Upgrade()
{
	UpgradeLevel++;
}

// 플레이어 시선이 닿을 때 — 강조 표시
void ATGMountedTower::OnFocused_Implementation(ATGPlayer* Player)
{
	// 외곽선 강조 표시 (커스텀 뎁스 패스 활성화)
	WeaponMesh->SetRenderCustomDepth(true);

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
	// 외곽선 강조 해제
		WeaponMesh->SetRenderCustomDepth(false);

	// 위젯 숨기기
	if (TowerWidget && TowerWidget->IsInViewport())
	{
		TowerWidget->RemoveFromParent();
	}
}

// 플레이어가 상호작용 키를 눌렀을 때
void ATGMountedTower::OnInteract_Implementation(ATGPlayer* Player)
{
	// 상호작용 키 누르면 업그레이드
	Upgrade();

	// UI 갱신
	if (TowerWidget)
	{
		TowerWidget->RefreshUI();
	}
}
