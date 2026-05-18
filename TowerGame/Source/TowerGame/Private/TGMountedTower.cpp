#include "TGMountedTower.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "BaseTower/TGTowerWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Core/GameFlow/TGGameMode.h"
#include "Enemies/TGNavigationManager.h"

ATGMountedTower::ATGMountedTower()
{
	PrimaryActorTick.bCanEverTick = true;

	//	몸체 메쉬 — 루트 컴포넌트로 설정
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	SetRootComponent(BodyMesh);
	InteractionCollision->SetupAttachment(BodyMesh);

	// 무기 메시
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(BodyMesh);
	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(WeaponMesh);

	//	외곽선 표현을 위한 커스텀 뎁스 스텐실 등록
	WeaponMesh->SetCustomDepthStencilValue(1);
	BodyMesh->SetCustomDepthStencilValue(1);

	//	FloatingUI: 타워 위에 떠있는 월드 위젯
	FloatingWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("FloatingWidget"));
	FloatingWidgetComp->SetupAttachment(BodyMesh);
	FloatingWidgetComp->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	FloatingWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	FloatingWidgetComp->SetDrawSize(FVector2D(200.f, 150.f));
	FloatingWidgetComp->SetVisibility(false);

	//	기본적으로 인터랙션을 끕니다
	SetInteractionEnabled(false);
}

void ATGMountedTower::BeginPlay()
{
	Super::BeginPlay();

	SetInteractionEnabled(true);
	ATGNavigationManager::Get(this)->NotifyBuildingPlaced();
	
	if (TowerWidgetClass)
	{
		FloatingWidgetComp->SetWidgetClass(TowerWidgetClass);
		TowerWidget = Cast<UTGTowerWidget>(FloatingWidgetComp->GetUserWidgetObject());
		if (TowerWidget)
		{
			TowerWidget->SetOwnerTower(this);
		}
	}
}

void ATGMountedTower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FollowTarget();
}

void ATGMountedTower::FollowTarget()
{
	if (Target == nullptr) return;

	FVector Direction = Target->GetActorLocation() - WeaponMesh->GetComponentLocation();
	Direction.Normalize();

	FRotator LookAtRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
	WeaponMesh->SetWorldRotation(LookAtRotation);
}

void ATGMountedTower::Upgrade()
{
	UpgradeLevel++;
}

// 플레이어 시선이 닿을 때 — 외곽선 + FloatingUI 표시
void ATGMountedTower::OnFocused_Implementation(ATGPlayer* Player)
{
	WeaponMesh->SetRenderCustomDepth(true);
	BodyMesh->SetRenderCustomDepth(true);

	if (TowerWidget)
	{
		TowerWidget->RefreshUI();
	}
	FloatingWidgetComp->SetVisibility(true);
}

// 플레이어 시선이 벗어날 때 — 외곽선 + FloatingUI 숨김
void ATGMountedTower::OnUnfocused_Implementation(ATGPlayer* Player)
{
	WeaponMesh->SetRenderCustomDepth(false);
	BodyMesh->SetRenderCustomDepth(false);

	FloatingWidgetComp->SetVisibility(false);
}

// 플레이어가 상호작용 키를 눌렀을 때 — 업그레이드만 담당
void ATGMountedTower::OnInteract_Implementation(ATGPlayer* Player)
{
	ATGGameMode* GM = Cast<ATGGameMode>(GetWorld()->GetAuthGameMode());
	if (!GM || !GM->SpendEnergy(20)) return;

	Upgrade();

	if (TowerWidget)
	{
		TowerWidget->RefreshUI();
	}
}
