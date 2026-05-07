#include "BaseTower/TGBaseTower.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NavModifierComponent.h"
#include "TGMountedTower.h"
#include "TGWeaponTower.h"
#include "NavAreas/NavArea_Null.h"
#include "Components/StaticMeshComponent.h"

ABaseTower::ABaseTower()
{
	// 발판은 움직이지 않으니 틱을 끔(최적화)
	PrimaryActorTick.bCanEverTick = false;

	// 발판메시
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetupAttachment(RootComponent);

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
}

void ABaseTower::BeginPlay()
{
	Super::BeginPlay();

	DynamicMaterial = BaseMesh->CreateDynamicMaterialInstance(0);
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

	// 반투명 처리
	if (DynamicMaterial)
	{
		DynamicMaterial->SetScalarParameterValue(TEXT("Opacity"), 0.5f);
		UE_LOG(LogTemp, Warning, TEXT("Setting Opacity"));
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

	// 불투명하게 복구
	if (DynamicMaterial)
	{
		DynamicMaterial->SetScalarParameterValue(TEXT("Opacity"), 1.0f);
	}

	//	인터랙션을 활성화합니다.
	SetInteractionEnabled(true);
}

void ABaseTower::OnFocused_Implementation(ATGPlayer* Player)
{
	Super::OnFocused_Implementation(Player);
}

void ABaseTower::OnUnfocused_Implementation(ATGPlayer* Player)
{
	Super::OnUnfocused_Implementation(Player);
}

void ABaseTower::OnInteract_Implementation(ATGPlayer* Player)
{
	if (bAttached)	return;
	Super::OnInteract_Implementation(Player);

	//	공격 타워 올리기
	AttachedWeapon = GetWorld()->SpawnActor<ATGWeaponTower>();
	bAttached = true;
	AttachedWeapon->SetActorLocation(GetActorLocation() + MountPoint->GetRelativeLocation());

	//	우선은 인터랙션 비활성화
	//	TODO : 추후 타워 파괴 기능 생겼을 때 재건축을 위해 활성화 하는 코드 추가해야함
	SetInteractionEnabled(false);
}
