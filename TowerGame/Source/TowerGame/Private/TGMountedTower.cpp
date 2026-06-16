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

	//	범위 표시 메쉬
	RangeSphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RangeSphere"));
	RangeSphere->SetupAttachment(BodyMesh);
	RangeSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RangeSphere->SetCanEverAffectNavigation(false);

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
	// NavigationManager가 없는 레벨(튜토리얼 등)에서도 동작하도록 null 검사
	if (ATGNavigationManager* NavigationManager = ATGNavigationManager::Get(this))
	{
		NavigationManager->NotifyBuildingPlaced();
	}

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

void ATGMountedTower::SetRangeSphereScale(float NewScale)
{
	// 기본 스피어 메시의 반지름은 50 유닛이므로, 원하는 범위 반지름에 맞게 나눠서 스케일 적용
	// 부모 컴포넌트의 월드 스케일을 추가로 나눠 상속된 스케일을 상쇄
	const float MeshRadius = 50.f;
	const float ParentScale = RangeSphere->GetAttachParent()->GetComponentScale().X;
	const float Scale = (NewScale / MeshRadius) / ParentScale;
	RangeSphere->SetRelativeScale3D(FVector(Scale));
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
	if (!GM || !GM->TryConsumeBuildToken()) return;

	Upgrade();

	if (TowerWidget)
	{
		TowerWidget->RefreshUI();
	}
}
