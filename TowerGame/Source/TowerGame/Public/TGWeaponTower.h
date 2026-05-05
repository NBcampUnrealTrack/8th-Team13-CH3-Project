#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TGWeaponTower.generated.h"

class ABaseTower;

UCLASS()
class TOWERGAME_API ATGWeaponTower : public AActor
{
	GENERATED_BODY()

public:
	ATGWeaponTower();

	// 무기 외형 메시
	UPROPERTY(VisibleAnywhere, Category = "Tower|Weapon")
	class UStaticMeshComponent* WeaponMesh;

	// 미리보기 모드 (반투명)
	void SetPreviewMode();

	// 베이스 타워 위에 설치 (중앙 부착)
	void FinalizeInstallation(ABaseTower* TargetBase);

protected:
	UPROPERTY()
	class UMaterialInstanceDynamic* DynamicMaterial;
};
