#include "TGMountedTower.h"
#include "Components/StaticMeshComponent.h"

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
}

// 플레이어 시선이 벗어날 때 — 강조 해제
void ATGMountedTower::OnUnfocused_Implementation(ATGPlayer* Player)
{
	if (DynamicMaterial)
	{
		DynamicMaterial->SetScalarParameterValue(TEXT("Highlight"), 0.0f);
	}
}

// 플레이어가 상호작용 키를 눌렀을 때
void ATGMountedTower::OnInteract_Implementation(ATGPlayer* Player)
{
	
}
