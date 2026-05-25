// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TGPatternBase.generated.h"

class ATGBossBase;
class UTGBossPhaseBase;

UCLASS(Abstract)
class TOWERGAME_API UTGPatternBase : public UObject
{
	GENERATED_BODY()

public:
	UTGPatternBase();

	virtual void Initialize(
		UTGBossPhaseBase* InOwnerPhase,
		ATGBossBase* InOwnerBoss,
		float InWarningRadius,
		float InAttackDamage,
		float InAttackEffectScale);

	// Pattern 실행 시작
	virtual void StartPattern(float WarningDrawTime) PURE_VIRTUAL(UTGPatternBase::StartPattern, );

	// Pattern 내부 타이머 정리
	virtual void StopPattern() PURE_VIRTUAL(UTGPatternBase::StopPattern, );

	// 공격 위치 계산
	virtual void GetAttackLocation();

	//공격 실행
	virtual void ExecuteAttack();

protected:
	void SpawnCircleWarning(const FVector& Location, float Radius, float LifeTime) const;
	AActor* SpawnSphereWarning(const FVector& Location, float Radius, float LifeTime = 0.f) const;

	// 범위 내 대상 탐색
	virtual void CollectDamageTargets(TArray<AActor*>& OutTargets) const;

	bool PrepareOverlapQuery(
		UWorld*& OutWorld,
		TArray<TEnumAsByte<EObjectTypeQuery>>& OutObjectTypes,
		TArray<AActor*>& OutIgnoreActors
	) const;

	// 데미지 대상 설정
	bool IsValidDamageTarget(AActor* TargetActor) const;

	// Sound / Effect
	void PlayAttackFeedback(const FVector& Location) const;

	//탐색된 대상에게 데미지 적용
	void ApplyDamageToTargets(const TArray<AActor*>& Targets, float Damage) const;

protected:
	UPROPERTY()
	TObjectPtr<UTGBossPhaseBase> OwnerPhase;

	UPROPERTY()
	TObjectPtr<ATGBossBase> OwnerBoss;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Pattern")
	float WarningRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Pattern")
	float AttackDamage;

	FVector AttackLocation;

	// 공격 범위 Material / Actor
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss|Pattern|Warning")
	TSubclassOf<AActor> SphereWarningActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss|Pattern|Warning")
	TObjectPtr<UMaterialInterface> CircleWarningMaterial;

	// 공격 Sound
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Pattern|Sound")
	TObjectPtr<USoundBase> AttackSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Pattern|Sound", meta = (ClampMin = "0.0"))
	float AttackSoundVolume;

	// 공격 Effect
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Pattern|Effect")
	TObjectPtr<UParticleSystem> AttackEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Pattern|Effect", meta = (ClampMin = "0.0"))
	float AttackEffectScale;
};
