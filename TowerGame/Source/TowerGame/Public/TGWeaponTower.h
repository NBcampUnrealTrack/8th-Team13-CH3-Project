#pragma once

#include "CoreMinimal.h"
#include "TGInteractiveActor.h"
#include "TGWeaponTower.generated.h"

UCLASS()
class TOWERGAME_API ATGWeaponTower : public ATGInteractiveActor
{
	GENERATED_BODY()

public:
	ATGWeaponTower();

protected:
	virtual void BeginPlay() override;

	// 공격 사거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower Settings")
	float AttackRange = 500.f;

	// 공격 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower Settings")
	float AttackDamage = 10.f;

	// 공격 간격 (초) — 기획: 2초당 1번
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower Settings")
	float DamageInterval = 2.0f;

	// 공격 반복 타이머 핸들
	FTimerHandle AttackTimerHandle;

	// 업그레이드 레벨
	UPROPERTY(BlueprintReadOnly, Category = "Tower Settings")
	int32 UpgradeLevel = 0;

	// 무기 메시
	UPROPERTY(VisibleAnywhere, Category = "Tower|Weapon")
	class UStaticMeshComponent* WeaponMesh;

	// 런타임에 색상을 변경하기 위한 다이나믹 머티리얼 인스턴스
	UPROPERTY()
	class UMaterialInstanceDynamic* DynamicMaterial;

public:
	// 공격 시작 (타워 설치 완료 시 호출)
	UFUNCTION(BlueprintCallable, Category = "Tower Logic")
	void StartAttack();

	// 공격 중지 (타워 파괴 시 호출)
	UFUNCTION(BlueprintCallable, Category = "Tower Logic")
	void StopAttack();

	// 범위 내 적을 스캔하고 데미지를 입히는 핵심 로직
	void ApplyRangeDamage();

	// 업그레이드 (데미지 1.5배 증가)
	UFUNCTION(BlueprintCallable, Category = "Tower Logic")
	void Upgrade();

	// 플레이어 시선이 닿을 때 — 강조 표시
	virtual void OnFocused_Implementation(ATGPlayer* Player) override;

	// 플레이어 시선이 벗어날 때 — 강조 해제
	virtual void OnUnfocused_Implementation(ATGPlayer* Player) override;

	// 플레이어가 상호작용 키를 눌렀을 때 — 업그레이드 UI 연결
	virtual void OnInteract_Implementation(ATGPlayer* Player) override;
};
