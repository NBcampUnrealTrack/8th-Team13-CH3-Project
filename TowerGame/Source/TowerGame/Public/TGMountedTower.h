#pragma once

#include "CoreMinimal.h"
#include "TGInteractiveActor.h"
#include "TGMountedTower.generated.h"

class UTGTowerWidget;

UCLASS()
class TOWERGAME_API ATGMountedTower : public ATGInteractiveActor
{
	GENERATED_BODY()

public:
	ATGMountedTower();

protected:
	// 무기 메시
	UPROPERTY(VisibleAnywhere, Category = "Tower|Base")
	class UStaticMeshComponent* WeaponMesh;

	// 런타임에 색상을 변경하기 위한 다이나믹 머티리얼 인스턴스
	UPROPERTY()
	class UMaterialInstanceDynamic* DynamicMaterial;

	// 업그레이드 레벨
	UPROPERTY(BlueprintReadOnly, Category = "Tower Settings")
	int32 UpgradeLevel = 0;

	// 위젯 클래스 (에디터에서 설정)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower|UI")
	TSubclassOf<UTGTowerWidget> TowerWidgetClass;

	// 런타임 위젯 인스턴스
	UPROPERTY()
	UTGTowerWidget* TowerWidget;

public:
	// 업그레이드 레벨 반환
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tower Settings")
	int32 GetUpgradeLevel() const { return UpgradeLevel; }

	// 업그레이드 (자식 클래스에서 재정의 가능)
	UFUNCTION(BlueprintCallable, Category = "Tower Logic")
	virtual void Upgrade();

	// 플레이어 시선이 닿을 때 — 강조 표시
	virtual void OnFocused_Implementation(ATGPlayer* Player) override;

	// 플레이어 시선이 벗어날 때 — 강조 해제
	virtual void OnUnfocused_Implementation(ATGPlayer* Player) override;

	// 플레이어가 상호작용 키를 눌렀을 때
	virtual void OnInteract_Implementation(ATGPlayer* Player) override;
};
