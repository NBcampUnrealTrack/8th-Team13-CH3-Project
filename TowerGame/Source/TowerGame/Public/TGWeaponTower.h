#pragma once

#include "CoreMinimal.h"
#include "TGMountedTower.h"
#include "TGWeaponTower.generated.h"

UCLASS()
class TOWERGAME_API ATGWeaponTower : public ATGMountedTower
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
	virtual void Upgrade() override;
};
