// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TGMissile.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class UProjectileMovementComponent;

//	미사일이 충돌함
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMissileHit, ATGMissile*, Missile, const FHitResult&, HitResult);
//	미사일이 만료됨(수명종료)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMissileExpired, ATGMissile*, Missile);

// 발사 시 일괄 전달할 설정값
USTRUCT(BlueprintType)
struct FTGMissileParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Missile")
	float Damage = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Missile")
	float InitialSpeed = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Missile")
	float MaxSpeed = 2000.f;

	// 0이면 직선 발사, 값이 클수록 강하게 추적
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Missile")
	float HomingAcceleration = 3000.f;

	// 0이면 단일 충돌, 0 초과면 범위 폭발
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Missile")
	float ExplosionRadius = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Missile")
	float LifeSpan = 8.f;
};

UCLASS()
class TOWERGAME_API ATGMissile : public AActor
{
	GENERATED_BODY()

public:
	ATGMissile();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void LifeSpanExpired() override;

public:
	// 유도 타깃 설정 — nullptr이면 직선 비행
	UFUNCTION(BlueprintCallable, Category = "TowerGame|Missile")
	void SetHomingTargetComponent(USceneComponent* TargetComponent);
	UFUNCTION(BlueprintCallable, Category = "TowerGame|Missile")
	void SetHomingTargetActor(AActor* TargetActor);
	// 월드의 고정 위치를 향해 유도 (TurnRate: 초당 방향 보간 비율, ArrivalRadius: 폭발 판정 반경)
	UFUNCTION(BlueprintCallable, Category = "TowerGame|Missile")
	void SetHomingLocation(const FVector& TargetLocation, float TurnRate = 2.f, float ArrivalRadius = 150.f);

	// 파라미터 적용 후 발사 시작
	UFUNCTION(BlueprintCallable, Category = "TowerGame|Missile")
	void Launch(const FTGMissileParams& Params);

	// 발사 주체 설정
	UFUNCTION(BlueprintCallable, Category = "TowerGame|Missile")
	void SetDamageInstigator(AActor* InInstigator);

public:
	UPROPERTY(BlueprintAssignable, Category = "TowerGame|Missile|Events")
	FOnMissileHit OnMissileHit;

	UPROPERTY(BlueprintAssignable, Category = "TowerGame|Missile|Events")
	FOnMissileExpired OnMissileExpired;

protected:
	UFUNCTION()
	void OnProjectileStopped(const FHitResult& ImpactResult);

	void Explode(const FHitResult& Hit);
	void ApplyExplosionDamage(const FVector& Location);
	void PlayHitEffects(const FVector& Location, const FVector& Normal);

protected:
	//	충돌범위
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TowerGame|Missile|Components")
	TObjectPtr<USphereComponent> CollisionComponent;

	//	발사체 메쉬
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TowerGame|Missile|Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	//	꼬리 이펙트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TowerGame|Missile|Components")
	TObjectPtr<UNiagaraComponent> TrailEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TowerGame|Missile|Movement")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	// BP 서브클래스에서 에셋 지정
	//	이펙트
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TowerGame|Missile|Effects")
	TObjectPtr<UNiagaraSystem> ExplosionEffect;
	//	사운드
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TowerGame|Missile|Sound")
	TObjectPtr<USoundBase> ExplosionSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TowerGame|Missile|Sound", meta = (ClampMin = "0.0"))
	float ExplosionSoundVolume = 1.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TowerGame|Missile|Stat")
	float Damage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TowerGame|Missile|Stat")
	float ExplosionRadius;

	UPROPERTY()
	TObjectPtr<AActor> DamageInstigator;

private:
	bool bHasExploded = false;

	// SetHomingLocation 호출 시 Tick 활성화
	bool bUseLocationHoming = false;
	FVector HomingWorldLocation = FVector::ZeroVector;
	float HomingTurnRate = 2.f;
	float HomingArrivalRadius = 150.f;
};
