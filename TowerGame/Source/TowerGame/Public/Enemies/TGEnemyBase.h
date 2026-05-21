// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Navigation/PathFollowingComponent.h"
#include "TGEnemyBase.generated.h"

class ABaseTower;
struct FAIRequestID;
class ATGNavigationManager;
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

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	TObjectPtr<ATGNavigationManager> NavigationManager;

public:
	void InitializeEnemy(ATGNavigationManager* InNavigationManager);
	FVector GetNavigationHeightOffset() const;

	// 경로
	virtual void RequestRepath();
	void SetNavigationManager(ATGNavigationManager* InNavigationManager);

	// Structure 공격
	virtual void StartStructureAttack();

	// 공격 범위 기준 경로 이동
	void StopStructureAttackRangeCheck();
	void CheckStructureAttackRange();

	// 공격 범위 X 경로 이동
	void MoveDirectlyToStructureTarget();

	void AttackStructureTarget();
	void StopStructureAttack();

	virtual float TakeDamage(
		float DamageAmount,
		const FDamageEvent& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser
	) override;
public:
	// 델리게이트 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Enemy|Stat")
	FOnEnemyHpChanged OnEnemyHpChanged;

	UPROPERTY(BlueprintAssignable, Category = "Enemy")
	FOnEnemyRemoved OnEnemyRemoved;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Enemy")
	FString GetEnemyType();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Enemy|Stat")
	float GetCurrentHP();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Enemy|Stat")
	float GetMaxHP();

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Drop")
	int32 EnergyDropAmount = 30;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	FString EnemyType;

	// Structure 공격 관련 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Attack")
	float StructureAttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Attack")
	float StructureAttackInterval;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Attack", meta = (ClampMin = "50.0"))
	float StructureAttackRange;

	// Sound
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Sound")
	TObjectPtr<USoundBase> StructureAttackSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Sound")
	TObjectPtr<USoundBase> DeathSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Sound")
	float SoundVolume;

	// Effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Effect")
	TObjectPtr<UNiagaraSystem> StructureAttackEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Effect")
	float EffectScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Effect")
	TObjectPtr<UParticleSystem> DeathEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Effect")
	float DeathEffectScale;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Stat")
	float MaxHP;

	UPROPERTY()
	float CurrentHP;

	// FlyingEnemy 위치 보정을 위한 FVector
	UPROPERTY(VisibleAnywhere, Category = "Enemy|Spawn")
	FVector NavigationHeightOffset;

	UPROPERTY()
	TObjectPtr<AActor> CurrentStructureTarget;

	FTimerHandle StructureAttackTimerHandle;
	FTimerHandle StructureAttackRangeCheckTimerHandle;

private:
	int32 GridSize;

protected:
	UFUNCTION()
	void HandleMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	bool TryRecoverToNearestNavMesh();

	bool IsStructureTargetInAttackRange(const AActor* Target) const;

private:
	// Structure 공격 이펙트 위치 계산
	USceneComponent* FindTargetEffectAttachComponent(AActor* Target) const;
	FVector GetRandomCollisionSurfaceLocation(UPrimitiveComponent* Component) const;

	// 가까운 건물 반환 추후 우선도 고려
	bool MoveToBlockingBuilding();
	bool TryMoveToAttackRangeOfBuilding(ABaseTower* Building);


protected:
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

	//	파츠를 분리합니다
	UFUNCTION(BlueprintCallable, Category="TowerGame|BodyParts")
	void DestroyUnit();
};
