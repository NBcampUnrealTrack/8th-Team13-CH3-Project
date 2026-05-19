// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TGBossBase.generated.h"

class ATGPlayer;
class UTGBossPhaseBase;

// 보스 체력, 삭제 이벤트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBossHpChanged, float, CurrentHP, float, MaxHP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossRemoved, ATGBossBase*, RemovedBoss);

UCLASS()
class TOWERGAME_API ATGBossBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATGBossBase();

protected:
	// Unreal LifeCycle
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual float TakeDamage(
		float DamageAmount,
		const FDamageEvent& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser
	) override;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Boss|Stat")
	FText GetBossName() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Boss|Stat")
	float GetCurrentHP() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Boss|Stat")
	float GetMaxHP() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Boss|Stat")
	float GetCurrentPhaseMinHP() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Boss|Stat")
	float GetCurrentPhaseMaxHP() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Boss|Spawn")
	float GetSpawnClearRadius() const;

	// 코드 책임 상 Player는 Phase보다 BossBase가 들고 있는게 바람직함
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Boss|Target")
	ATGPlayer* GetPlayer() const;

	// 파괴 가능한 Part지정
	void SetActiveBreakablePartTags(const TArray<FName>& InBreakablePartTags);

public:
	UPROPERTY(BlueprintAssignable, Category = "Boss|Stat")
	FOnBossHpChanged OnBossHpChanged;

	UPROPERTY(BlueprintAssignable, Category = "Boss")
	FOnBossRemoved OnBossRemoved;

protected:
	// PhaseClasses[Index]를 현재 State 객체로 생성하고 전환한다.
	void ChangeToNextPhase();
	// 현재 HP 비율이 PhaseHPRatio 기준 이하인지 확인한다.
	void CheckPhaseTransition();

	// Boss 본체 HP만 감소시킨다. 부위 파괴는 이후 단계에서 분리한다.
	void ApplyBossDamage(float DamageAmount);
	float ApplyBreakablePartDamage(UActorComponent* HitComponent, float DamageAmount);

	// 부위 파괴
	void DestroyBreakableParts(FName PartTag);
	void DestroyDetachedPartComponent(UStaticMeshComponent* StaticMesh, FName PartTag);
	// Phase 전환 시 해당 페이즈의 파괴되지 않은 부위 파괴
	void DestroyDetachedPartComponent();

	// Material 초기화 및 갱신
	void RebuildPartDamageMaterialCache();
	void UpdateBreakablePartDamageVisual(FName PartTag, float CurrentPartHP, float MaxPartHP);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss")
	FText BossName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Stat")
	float MaxHP;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Stat")
	float CurrentHP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Stat")
	float TowerDamageMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Spawn")
	float SpawnClearRadius;

	// 파생 Boss가 사용할 Phase 클래스를 순서대로 등록한다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Phase")
	TArray<TSubclassOf<UTGBossPhaseBase>> PhaseClasses;

	// 현재 실행 중인 State 객체.
	UPROPERTY()
	TObjectPtr<UTGBossPhaseBase> CurrentPhase;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Phase")
	int32 CurrentPhaseIndex;

	// Index N은 Phase N에서 Phase N+1로 넘어가는 HP 비율이다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Phase")
	TArray<float> PhaseHPRatio;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Part")
	TSet<FName> ActiveBreakablePartTags;

	// 부위 파괴 관련 설정
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss|Part")
	float PartsLifeSpan;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss|Part")
	float ExplodeRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss|Part")
	float ExplodeForce;

	// 싱글 플레이 기준 Boss가 공격 대상으로 사용할 Player.
	UPROPERTY()
	TObjectPtr<ATGPlayer> TargetPlayer;

	FTimerHandle PhaseTransitionTimerHandle;

private:
	// 부위 외형 손상용 캐시
	TMap<FName, TArray<UMaterialInstanceDynamic*>> PartDamageMaterialMap;

};
