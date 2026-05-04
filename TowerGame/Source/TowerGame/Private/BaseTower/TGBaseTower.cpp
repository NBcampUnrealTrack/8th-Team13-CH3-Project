#include "BaseTower/TGBaseTower.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NavModifierComponent.h"
#include "NavAreas/NavArea_Null.h"
#include "Components/StaticMeshComponent.h"

ABaseTower::ABaseTower()
{
	// 발판은 움직이지 않으니 틱을 끔(최적화)
	PrimaryActorTick.bCanEverTick = false;

	// 발판메시
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = BaseMesh;

	// 길막 (기본값)
	BaseMesh->SetCollisionProfileName(TEXT("BlockAll"));

	// Navigation 차단영역
	// 건물 배치 시 함께 생성
	NavModifier = CreateDefaultSubobject<UNavModifierComponent>(TEXT("NavModifier"));

	if (NavModifier)
	{
		// 적들이 지나갈 수 없는 영역으로 만들기
		NavModifier->SetAreaClass(UNavArea_Null::StaticClass());

	}

	// 무기 설치지점
	MountPoint = CreateDefaultSubobject<USceneComponent>(TEXT("MountPoint"));
	MountPoint->SetupAttachment(RootComponent);

	// 큐브 높이에 맞춰서 설치 위치를 윗면(50.f)으로 고정
	MountPoint->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
}

void ABaseTower::SetPreviewMode()
{
	// 충돌끄기
	BaseMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 배치 중에는 내비게이션을 막지 않도록 컴포넌트 비활성화 (선택 사항)
	if (NavModifier)
	{
		NavModifier->SetComponentTickEnabled(false);
	}

	// 반투명 처리
	DynamicMaterial = BaseMesh->CreateDynamicMaterialInstance(0);

	if (DynamicMaterial)
	{
		DynamicMaterial->SetScalarParameterValue(TEXT("Opacity"), 0.5f);
	}
}

void ABaseTower::FinalizeInstallation()
{
	// 충돌 켜기
	BaseMesh->SetCollisionProfileName(TEXT("BlockAll"));
	BaseMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// 설치가 완료되면 내비게이션 차단 영역 활성화
	if (NavModifier)
	{
		NavModifier->SetComponentTickEnabled(true);
	}

	// 불투명하게 복구
	if (DynamicMaterial)
	{
		DynamicMaterial->SetScalarParameterValue(TEXT("Opacity"), 1.0f);
	}
}
