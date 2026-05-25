// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TGMissile.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UNiagaraComponent;
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
	float InitialSpeed = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Missile")
	float MaxSpeed = 2000.f;

	// 0이면 직선 발사, 값이 클수록 강하게 추적
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Missile")
	float HomingAcceleration = 3000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Missile")
	float LifeSpan = 8.f;
};

UCLASS()
class TOWERGAME_API ATGMissile : public AActor
{
	GENERATED_BODY()

public:
	ATGMissile();

	virtual float TakeDamage(
		float DamageAmount,
		const FDamageEvent& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser
	) override;

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
	void Launch(const FTGMissileParams& Params, float CollisionDisableTime = 0.4f);

public:
	UPROPERTY(BlueprintAssignable, Category = "TowerGame|Missile|Events")
	FOnMissileHit OnMissileHit;

	UPROPERTY(BlueprintAssignable, Category = "TowerGame|Missile|Events")
	FOnMissileExpired OnMissileExpired;

protected:
	UFUNCTION()
	void OnProjectileStopped(const FHitResult& ImpactResult);

	void Explode(const FHitResult& Hit);

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

private:
	bool bHasExploded = false;

	// SetHomingLocation 호출 시 Tick 활성화
	bool bUseLocationHoming = false;
	FVector HomingWorldLocation = FVector::ZeroVector;
	float HomingTurnRate = 2.f;
	float HomingArrivalRadius = 150.f;
};
