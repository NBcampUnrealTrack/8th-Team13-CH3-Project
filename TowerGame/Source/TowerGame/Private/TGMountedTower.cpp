#include "TGMountedTower.h"
#include "Components/StaticMeshComponent.h"
#include "BaseTower/TGTowerWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Core/GameFlow/TGGameMode.h"

ATGMountedTower::ATGMountedTower()
{
	PrimaryActorTick.bCanEverTick = true;

	//	몸체 메쉬
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(RootComponent);

	// 무기 메시
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(BodyMesh);
	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(WeaponMesh);

	//	외곽선 표현을 위한 커스텀 뎁스 스텐실 등록
	WeaponMesh->SetCustomDepthStencilValue(1);

	//	기본적으로 인터랙션을 끕니다
	SetInteractionEnabled(false);
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

// 플레이어 시선이 닿을 때 — 강조 표시
void ATGMountedTower::OnFocused_Implementation(ATGPlayer* Player)
{
	//	커스텀 뎁스 패스에 등록 -> 포스트 프로세싱 가능
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
	//	커스텀 뎁스 패스에서 제외
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
	//	게임모드에서 자원 차감 — 자원 부족 시 업그레이드 불가
	ATGGameMode* GM = Cast<ATGGameMode>(GetWorld()->GetAuthGameMode());
	if (!GM) return;
	if (!GM->SpendEnergy(20)) return;

	//	상호작용 키 누르면 업그레이드
	Upgrade();

	// UI 갱신
	if (TowerWidget)
	{
		TowerWidget->RefreshUI();
	}
}
