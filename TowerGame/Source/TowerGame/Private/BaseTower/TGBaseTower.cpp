#include "BaseTower/TGBaseTower.h"
#include "Core/GameFlow/TGGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NavModifierComponent.h"
#include "TGMountedTower.h"
#include "NavAreas/NavArea_Null.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

ABaseTower::ABaseTower()
{
	// 발판은 움직이지 않으니 틱을 끔(최적화)
	PrimaryActorTick.bCanEverTick = false;

	// 발판메시 — 루트 컴포넌트로 설정
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	SetRootComponent(BaseMesh);
	InteractionCollision->SetupAttachment(BaseMesh);

	// 길막 (기본값)
	BaseMesh->SetCollisionProfileName(TEXT("BlockAll"));
	BaseMesh->SetCanEverAffectNavigation(false);

	// Navigation 차단영역
	// 건물 배치 시 함께 생성
	NavModifier = CreateDefaultSubobject<UNavModifierComponent>(TEXT("NavModifier"));
	if (NavModifier)
	{
		NavModifier->SetAutoActivate(false);
	}

	// 무기 설치지점
	MountPoint = CreateDefaultSubobject<USceneComponent>(TEXT("MountPoint"));
	MountPoint->SetupAttachment(BaseMesh);

	// 큐브 높이에 맞춰서 설치 위치를 윗면(50.f)으로 고정
	MountPoint->SetRelativeLocation(FVector(0.f, 0.f, 50.f));

	//	기본적으로 인터랙션을 끕니다
	SetInteractionEnabled(false);

	//	외곽선 표현을 위한 뎁스 스텐실 등록
	BaseMesh->SetCustomDepthStencilValue(1);
}

void ABaseTower::BeginPlay()
{
	Super::BeginPlay();

	//	투명도 조정용 다이나믹 머티리얼 인스턴스 생성
	PreviewMaterial = BaseMesh->CreateDynamicMaterialInstance(0);

	//	기본적으로 인터랙션을 끕니다
	SetInteractionEnabled(false);
}

void ABaseTower::Disable()
{
	// 메쉬 숨기기
	BaseMesh->SetVisibility(false);

	// 충돌 끄기
	BaseMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 네비게이션 차단 끄기
	if (NavModifier)
	{
		NavModifier->Deactivate();
	}
}

void ABaseTower::SetPreviewMode()
{
	// 메쉬 보이게
	BaseMesh->SetVisibility(true);

	// 충돌끄기
	BaseMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 배치 중에는 내비게이션을 막지 않도록 컴포넌트 비활성화 (선택 사항)
	if (NavModifier)
	{
		NavModifier->Deactivate();
	}

	//	미리보기 상태 — 투명도 조정용 머티리얼 적용
	BaseMesh->SetMaterial(0, PreviewMaterial);
	if (PreviewMaterial)
	{
		PreviewMaterial->SetScalarParameterValue(TEXT("Opacity"), 0.85f);
	}
}

void ABaseTower::FinalizeInstallation()
{
	// 메쉬 보이게
	BaseMesh->SetVisibility(true);

	// 충돌 켜기
	BaseMesh->SetCollisionProfileName(TEXT("BlockAll"));
	BaseMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// 설치가 완료되면 내비게이션 차단 영역 활성화
	if (NavModifier)
	{
		NavModifier->SetAreaClass(UNavArea_Null::StaticClass());
		NavModifier->Activate();
	}

	//	설치 완료 — PBR 머티리얼로 교체
	if (PBRMaterial)
	{
		BaseMesh->SetMaterial(0, PBRMaterial);
	}

	//	인터랙션을 활성화합니다.
	SetInteractionEnabled(true);
}

void ABaseTower::ResetTower()
{
	if (AttachedWeapon)
	{
		AttachedWeapon->Destroy();
		AttachedWeapon = nullptr;
	}
	bAttached = false;
	Disable();
	SetInteractionEnabled(false);
}

void ABaseTower::OnFocused_Implementation(ATGPlayer* Player)
{
	Super::OnFocused_Implementation(Player);
	BaseMesh->SetRenderCustomDepth(true);
}

void ABaseTower::OnUnfocused_Implementation(ATGPlayer* Player)
{
	Super::OnUnfocused_Implementation(Player);
	BaseMesh->SetRenderCustomDepth(false);
}

void ABaseTower::OnInteract_Implementation(ATGPlayer* Player)
{
	if (bAttached) return;

	ATGGameMode* GM = Cast<ATGGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GM) return;

	// 선택된 타입이 없으면 무시
	if (SelectedTurretType == ETGTurretType::None) return;

	// 자원 차감
	if (!GM->SpendEnergy(50)) return;

	Super::OnInteract_Implementation(Player);

	// 선택된 타입으로 타워 스폰
	TSubclassOf<ATGMountedTower> TowerClass = GM->GetTowerSubclass(SelectedTurretType);
	if (!TowerClass) return;

	ATGMountedTower* MountedTower = GetWorld()->SpawnActor<ATGMountedTower>(TowerClass);
	if (MountedTower)
	{
		bAttached = true;
		AttachedWeapon = MountedTower;
		MountedTower->SetActorLocation(GetActorLocation() + MountPoint->GetRelativeLocation());
		MountedTower->SetInteractionEnabled(true);
		SetInteractionEnabled(false);
	}
}
