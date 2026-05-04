#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "NavModifierComponent.h"
#include "TGBaseTower.generated.h"

class UStaticMeshComponent;

UCLASS()
class TOWERGAME_API ABaseTower : public AActor
{
	GENERATED_BODY()

public:
	ABaseTower();

	// 기본 메시 (몸통)
	UPROPERTY(VisibleAnywhere, Category = "Tower|Base")
	UStaticMeshComponent* BaseMesh;

	// 내비게이션 차단 영역 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Navigation")
	UNavModifierComponent* NavModifier;

	// 무기를 부착할 포인트
	UPROPERTY(VisibleAnywhere, Category = "Tower|Base")
	USceneComponent* MountPoint;

	// 현재 부착된 무기 액터 참조 (필요할 경우)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tower|Base")
	AActor* AttachedWeapon;

	// 마우스로 배치하는 동안의 미리보기 모드 (반투명, 충돌 없음)
	void SetPreviewMode();

	// 클릭 후 최종 배치 완료 처리 (활성화, 바디 활성화)
	void FinalizeInstallation();

protected:
	// 런타임에 색상을 변경하기 위한 다이나믹 머티리얼 인스턴스
	UPROPERTY()
	class UMaterialInstanceDynamic* DynamicMaterial;
};
