// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemies/TGTargetable.h"
#include "TGSwarmEnemy.generated.h"

class UStaticMeshComponent;
class UTGCombatComponent;
class ATGNavigationManager;
class ATGCoreBase;
class UNiagaraSystem;
class USoundBase;
class UTGSwarmFlockSubsystem;

// 대량(수백 마리) 처리를 위한 경량 적.
// ACharacter/AIController/NavMesh 없이 AActor + 직접 벡터 이동으로 코어를 향해 진격한다.
// 전투(HP/데미지/사망/EXP)는 공용 UTGCombatComponent가 담당한다.
UCLASS()
class TOWERGAME_API ATGSwarmEnemy : public AActor, public ITGTargetable
{
	GENERATED_BODY()

public:
	ATGSwarmEnemy();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void Tick(float DeltaTime) override;

	// ITGTargetable
	virtual bool IsTargetable() const override;
	virtual void SetMoveSlowMultiplier(float Multiplier) override;

	virtual float TakeDamage(
		float DamageAmount,
		const FDamageEvent& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser
	) override;

protected:
	// 본체 메시 (BP에서 메시 에셋 지정 필요). 무기 트레이스(ECC_Visibility)를 막도록 설정됨.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComp;

	// 공용 전투 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTGCombatComponent> Combat;

	// 이동 속도 (cm/s)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Swarm|Move")
	float MoveSpeed = 250.f;

	// 경로 재탐색 간격 (초) — 새로 지어진 타워/벽 우회를 위해 주기적으로 재탐색
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Swarm|Move", meta = (ClampMin = "0.1"))
	float RepathInterval = 2.0f;

	// 경유점 도달 인정 반경
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Swarm|Move", meta = (ClampMin = "10.0"))
	float WaypointAcceptanceRadius = 60.f;

	// 분리(서로 밀어내기) 반경 — 이 안에 있는 다른 경량 적과 겹치지 않게 밀어낸다
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Swarm|Separation", meta = (ClampMin = "0.0"))
	float SeparationRadius = 120.f;

	// 분리력을 이동 방향에 섞는 가중치 (클수록 더 강하게 퍼짐)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Swarm|Separation", meta = (ClampMin = "0.0"))
	float SeparationWeight = 0.6f;

	// 코어 공격 사거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Swarm|Attack", meta = (ClampMin = "10.0"))
	float AttackRange = 150.f;

	// 코어 공격 데미지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Swarm|Attack")
	float AttackDamage = 1.f;

	// 코어 공격 간격 (초)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Swarm|Attack", meta = (ClampMin = "0.05"))
	float AttackInterval = 1.0f;

	// 사망 이펙트/사운드 (선택)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Swarm|FX")
	TObjectPtr<UNiagaraSystem> DeathEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Swarm|FX")
	TObjectPtr<USoundBase> DeathSound;

private:
	// 사망 처리
	UFUNCTION()
	void HandleDied(AActor* DeadActor);

	// 현재 목표 위치 (코어, 없으면 플레이어)
	bool GetTargetLocation(FVector& OutLocation) const;
	AActor* GetTargetActor() const;

	// 목적지까지 NavMesh 경로 재탐색. 성공 시 PathPoints 갱신.
	bool RecomputePath(const FVector& Destination);

	UPROPERTY()
	TObjectPtr<ATGNavigationManager> NavigationManager;

	UPROPERTY()
	TObjectPtr<UTGSwarmFlockSubsystem> FlockSubsystem;

	// 마지막 공격 이후 누적 시간
	float TimeSinceLastAttack = 0.f;

	// 이동 슬로우 배율 (1.0 = 정상, 디버프 타워가 변경)
	float SlowMultiplier = 1.f;

	// 현재 추종 중인 NavMesh 경로 경유점들
	TArray<FVector> PathPoints;

	// 현재 향하는 경유점 인덱스
	int32 CurrentPathIndex = 0;

	// 다음 경로 재탐색까지 남은 시간
	float RepathTimer = 0.f;
};
