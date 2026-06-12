// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TGEnemyBase.generated.h"

class ATGNavigationManager;
class UTGEnemyMovementComponent;
class UNiagaraSystem;
class USoundBase;
class UPrimitiveComponent;
class USceneComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnemyHpChanged, float, CurrentHP, float, MaxHP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyRemoved, ATGEnemyBase*, RemovedEnemy);

UCLASS()
class TOWERGAME_API ATGEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Unreal Life Cycle
	ATGEnemyBase();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// 델리게이트 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Enemy|Stat")
	FOnEnemyHpChanged OnEnemyHpChanged;

	UPROPERTY(BlueprintAssignable, Category = "Enemy")
	FOnEnemyRemoved OnEnemyRemoved;

	// 이동 로직
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTGEnemyMovementComponent> EnemyMovementComponent;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy")
	TObjectPtr<ATGNavigationManager> NavigationManager;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	FString EnemyType;

	// HP
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Stat")
	float MaxHP;

	UPROPERTY()
	float CurrentHP;

	// 사망 처리 중복 실행 방지 (사망 후 잔해 피격 시 에너지 중복 지급 차단)
	bool bDead = false;

	// Drop
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Drop")
	int32 EnergyDropAmount;

	// FlyingEnemy 위치 보정을 위한 FVector
	UPROPERTY(VisibleAnywhere, Category = "Enemy|Spawn")
	FVector NavigationHeightOffset;

protected:

	// Structure 공격 관련 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Attack")
	float StructureAttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Attack")
	float StructureAttackInterval;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Attack", meta = (ClampMin = "50.0"))
	float StructureAttackRange;

	FTimerHandle StructureAttackTimerHandle;

	//	사망시 파츠 파괴용
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="TowerGame|BodyParts")
	TArray<TObjectPtr<UStaticMeshComponent>> BodyParts;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="TowerGame|BodyParts")
	float PartsLifeSpan = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="TowerGame|BodyParts")
	float ExplodeRadius = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="TowerGame|BodyParts")
	float ExplodeForce = 500.f;

	FTimerHandle ExplodeTimerHandle;

protected:
	// Sound
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Sound")
	TObjectPtr<USoundBase> StructureAttackSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Sound")
	TObjectPtr<USoundBase> DeathSound;

	// Effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Effect")
	TObjectPtr<UNiagaraSystem> StructureAttackEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Effect")
	float EffectScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Effect")
	TObjectPtr<UParticleSystem> DeathEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Effect")
	float DeathEffectScale;

private:
	// 현재 이동 목표
	UPROPERTY()
	TObjectPtr<AActor> CurrentStructureTarget;

public:
	void InitializeEnemy(ATGNavigationManager* InNavigationManager);

	// 경로
	virtual void RequestRepath();
	void SetNavigationManager(ATGNavigationManager* InNavigationManager);

	// Structure 공격
	virtual void StartStructureAttack();
	void AttackStructureTarget();
	void StopStructureAttack();

	virtual float TakeDamage(
		float DamageAmount,
		const FDamageEvent& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser
	) override;

public:
	// Get
	FVector GetNavigationHeightOffset() const;
	float GetStructureAttackRange() const;

	UFUNCTION(BlueprintPure, Category = "Enemy")
	FString GetEnemyType() const;

	UFUNCTION(BlueprintPure, Category = "Enemy|Stat")
	float GetCurrentHP() const;

	UFUNCTION(BlueprintPure, Category = "Enemy|Stat")
	float GetMaxHP() const;

	UFUNCTION(BlueprintPure, Category = "Enemy|Stat")
	bool IsDead() const { return bDead; }

protected:
	UFUNCTION()
	void HandleAttackRangeReached(AActor* TargetActor);

	bool IsStructureTargetInAttackRange(const AActor* Target) const;

private:
	// Structure 공격 이펙트 위치 계산
	USceneComponent* FindTargetEffectAttachComponent(AActor* Target) const;
	FVector GetRandomCollisionSurfaceLocation(UPrimitiveComponent* Component) const;

	//	제거 - 파츠 분리
	void DestroyBodyParts();
};
