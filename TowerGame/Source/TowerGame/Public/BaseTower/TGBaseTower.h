#pragma once
#include "CoreMinimal.h"
#include "TGInteractiveActor.h"
#include "BaseTower/TGBuildWidget.h"
#include "BaseTower/TGTurretType.h"
#include "TGBaseTower.generated.h"

class ATGMountedTower;
class UNavModifierComponent;
class UStaticMeshComponent;

UCLASS()
class TOWERGAME_API ABaseTower : public ATGInteractiveActor
{
	GENERATED_BODY()

public:
	ABaseTower();
	virtual void BeginPlay() override;

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
	bool bAttached = false;

	// 그리드에 미리 배치 후 대기 상태 (완전 투명 / 충돌 X / 네비 X)
	void Disable();

	// 마우스로 배치하는 동안의 미리보기 모드 (반투명, 충돌 없음)
	void SetPreviewMode();

	// 클릭 후 최종 배치 완료 처리 (활성화, 바디 활성화)
	void FinalizeInstallation();

	// TGPlayer에서 숫자키로 선택한 타워 타입 전달받음
	void SetSelectedTurretType(ETGTurretType InType) { SelectedTurretType = InType; }

	// 플레이어 시선이 닿을 때
	virtual void OnFocused_Implementation(ATGPlayer* Player) override;
	// 플레이어 시선이 벗어날 때
	virtual void OnUnfocused_Implementation(ATGPlayer* Player) override;
	// 플레이어가 상호작용 키를 눌렀을 때
	virtual void OnInteract_Implementation(ATGPlayer* Player) override;

protected:
	// 투명도 조정용 다이나믹 머티리얼 (미리보기 상태에서 사용)
	UPROPERTY()
	class UMaterialInstanceDynamic* PreviewMaterial;

	// PBR 머티리얼 (설치 완료 후 사용)
	UPROPERTY(EditAnywhere, Category = "Tower|Base")
	class UMaterialInterface* PBRMaterial;

	// 위젯 클래스 — 에디터에서 WBP_BuildWidget 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower|UI")
	TSubclassOf<UTGBuildWidget> BuildWidgetClass;

	// 런타임 위젯 인스턴스
	UPROPERTY()
	UTGBuildWidget* BuildWidget;

	// 선택된 타워 타입 — TGPlayer에서 SetSelectedTurretType으로 전달받음
	ETGTurretType SelectedTurretType = ETGTurretType::None;
};
