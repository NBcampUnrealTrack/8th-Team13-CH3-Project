#include "TGWeaponTower.h"
#include "BaseTower/TGBaseTower.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

ATGWeaponTower::ATGWeaponTower()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;

	// 배치 전에는 마우스 트레이싱에 걸리지 않게 충돌을 끔
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ATGWeaponTower::SetPreviewMode()
{
	// 다이나믹 머티리얼 생성 및 반투명 설정
	DynamicMaterial = WeaponMesh->CreateDynamicMaterialInstance(0);
	if (DynamicMaterial)
	{
		// 머티리얼에 Opacity 파라미터가 있어야 작동함
		DynamicMaterial->SetScalarParameterValue(TEXT("Opacity"), 0.5f);
	}
}

void ATGWeaponTower::FinalizeInstallation(ABaseTower* TargetBase)
{
	if (!TargetBase || !TargetBase->MountPoint) return;

	// 1. 베이스 타워의 MountPoint에 딱 붙이기 (중앙 정렬)
	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(TargetBase->MountPoint, AttachRules);

	// 2. 불투명하게 복구
	if (DynamicMaterial)
	{
		DynamicMaterial->SetScalarParameterValue(TEXT("Opacity"), 1.0f);
	}

	// 3. 설치 완료 후 충돌 켜기
	WeaponMesh->SetCollisionProfileName(TEXT("BlockAll"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// 4. 베이스 타워에 이 무기를 등록 (헤더에 선언하신 변수 활용)
	TargetBase->AttachedWeapon = this;
}
