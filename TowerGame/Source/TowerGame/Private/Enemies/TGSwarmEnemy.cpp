// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemies/TGSwarmEnemy.h"
#include "Enemies/TGCombatComponent.h"
#include "Enemies/TGNavigationManager.h"
#include "Enemies/TGSwarmFlockSubsystem.h"
#include "Enemies/TGCoreBase.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

ATGSwarmEnemy::ATGSwarmEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	SetRootComponent(MeshComp);

	// 무기 라인트레이스(ECC_Visibility)에 걸리도록 설정. 그 외 충돌은 무시해 대량 처리 시 부하/간섭 최소화.
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MeshComp->SetCollisionObjectType(ECC_WorldDynamic);
	MeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	MeshComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	MeshComp->SetGenerateOverlapEvents(false);
	MeshComp->SetCanEverAffectNavigation(false);

	Combat = CreateDefaultSubobject<UTGCombatComponent>(TEXT("Combat"));
}

void ATGSwarmEnemy::BeginPlay()
{
	Super::BeginPlay();

	NavigationManager = ATGNavigationManager::Get(this);

	// 타워가 타겟할 수 있도록 레지스트리에 등록
	if (NavigationManager)
	{
		NavigationManager->RegisterTargetable(this);
	}

	// 분리(겹침 방지) 서브시스템에 등록
	if (UWorld* World = GetWorld())
	{
		FlockSubsystem = World->GetSubsystem<UTGSwarmFlockSubsystem>();
		if (FlockSubsystem)
		{
			FlockSubsystem->RegisterAgent(this);
		}
	}

	// 다수의 적이 같은 프레임에 동시에 경로탐색하지 않도록 초기 타이머를 분산
	RepathTimer = FMath::FRandRange(0.f, RepathInterval);

	if (Combat)
	{
		Combat->OnDied.AddUniqueDynamic(this, &ATGSwarmEnemy::HandleDied);
	}
}

void ATGSwarmEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 제거/사망 시 타겟 레지스트리에서 해제
	if (NavigationManager)
	{
		NavigationManager->UnRegisterTargetable(this);
	}

	if (FlockSubsystem)
	{
		FlockSubsystem->UnregisterAgent(this);
	}

	Super::EndPlay(EndPlayReason);
}

bool ATGSwarmEnemy::IsTargetable() const
{
	return Combat && !Combat->IsDead();
}

void ATGSwarmEnemy::SetMoveSlowMultiplier(float Multiplier)
{
	SlowMultiplier = FMath::Clamp(Multiplier, 0.f, 1.f);
}

void ATGSwarmEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Combat && Combat->IsDead()) return;

	FVector TargetLocation;
	if (!GetTargetLocation(TargetLocation)) return;

	const FVector CurrentLocation = GetActorLocation();

	// 실제 목표(코어)까지의 수평 거리로 공격/이동을 판정 (경로는 이동에만 사용)
	FVector ToTarget = TargetLocation - CurrentLocation;
	ToTarget.Z = 0.f;

	if (ToTarget.SizeSquared() <= FMath::Square(AttackRange))
	{
		// 사거리 안 — 경로 정리 후 주기적으로 코어 공격
		PathPoints.Reset();

		TimeSinceLastAttack += DeltaTime;
		if (TimeSinceLastAttack >= AttackInterval)
		{
			TimeSinceLastAttack = 0.f;
			if (AActor* Target = GetTargetActor())
			{
				UGameplayStatics::ApplyDamage(Target, AttackDamage, nullptr, this, UDamageType::StaticClass());
			}
		}

		// 코어 앞에서 한 점에 포개지지 않도록 분리력만 적용 (서로 밀어내 링 형태로 퍼짐)
		if (FlockSubsystem && SeparationRadius > 0.f)
		{
			FVector Sep = FlockSubsystem->ComputeSeparation(this, CurrentLocation, SeparationRadius);
			Sep.Z = 0.f;
			if (!Sep.IsNearlyZero())
			{
				SetActorLocation(CurrentLocation + Sep.GetSafeNormal() * MoveSpeed * SlowMultiplier * DeltaTime, false);
			}
		}
		return;
	}

	// 도착 즉시 첫 타격이 나가도록 준비
	TimeSinceLastAttack = AttackInterval;

	// 주기적 경로 재탐색 — 새 타워/벽을 우회하기 위함.
	// 실패해도 타이머로만 재시도(매 틱 탐색 방지). 경로 없는 동안은 아래에서 직진 폴백.
	RepathTimer -= DeltaTime;
	if (RepathTimer <= 0.f)
	{
		RecomputePath(TargetLocation);
		RepathTimer = RepathInterval;
	}

	// 향할 지점 결정: 유효한 경유점이 있으면 그쪽, 없으면 목표로 직진(폴백)
	FVector MoveGoal = TargetLocation;
	if (PathPoints.IsValidIndex(CurrentPathIndex))
	{
		FVector ToWaypoint = PathPoints[CurrentPathIndex] - CurrentLocation;
		ToWaypoint.Z = 0.f;
		if (ToWaypoint.SizeSquared() <= FMath::Square(WaypointAcceptanceRadius))
		{
			// 경유점 도달 → 다음 경유점
			CurrentPathIndex++;
		}
		MoveGoal = PathPoints.IsValidIndex(CurrentPathIndex) ? PathPoints[CurrentPathIndex] : TargetLocation;
	}

	// 수평 방향으로만 진격 (지면 적 가정 — Z는 유지)
	FVector Dir = MoveGoal - CurrentLocation;
	Dir.Z = 0.f;
	Dir = Dir.GetSafeNormal();

	// 이웃과 겹치지 않도록 분리력을 이동 방향에 혼합 (결과는 정규화하므로 속도는 일정)
	if (FlockSubsystem && SeparationRadius > 0.f && SeparationWeight > 0.f && !Dir.IsNearlyZero())
	{
		FVector Sep = FlockSubsystem->ComputeSeparation(this, CurrentLocation, SeparationRadius);
		Sep.Z = 0.f;
		FVector Blended = Dir + Sep * SeparationWeight;
		if (!Blended.IsNearlyZero())
		{
			Dir = Blended.GetSafeNormal();
		}
	}

	if (!Dir.IsNearlyZero())
	{
		// 디버프 슬로우 배율 반영
		SetActorLocation(CurrentLocation + Dir * MoveSpeed * SlowMultiplier * DeltaTime, false);
		SetActorRotation(Dir.Rotation());
	}
}

bool ATGSwarmEnemy::RecomputePath(const FVector& Destination)
{
	PathPoints.Reset();
	CurrentPathIndex = 0;

	UWorld* World = GetWorld();
	if (!World) return false;

	// 기존 NavMesh 사용 (타워가 NavModifier로 카빙한 경로를 그대로 따라감)
	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(
		World, GetActorLocation(), Destination, this);

	if (NavPath && NavPath->IsValid() && NavPath->PathPoints.Num() > 0)
	{
		PathPoints = NavPath->PathPoints;
		// 0번 경유점은 현재 위치이므로 1번부터 추종 (경유점이 하나뿐이면 0번)
		CurrentPathIndex = (PathPoints.Num() > 1) ? 1 : 0;
		return true;
	}

	return false;
}

float ATGSwarmEnemy::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float Base = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (Combat)
	{
		return Combat->ApplyDamage(DamageAmount);
	}
	return Base;
}

void ATGSwarmEnemy::HandleDied(AActor* DeadActor)
{
	if (DeathEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DeathEffect, GetActorLocation());
	}
	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}

	Destroy();
}

AActor* ATGSwarmEnemy::GetTargetActor() const
{
	// 1순위: 코어 (타워디펜스 목표). 없으면 플레이어로 폴백.
	if (NavigationManager)
	{
		if (ATGCoreBase* Core = NavigationManager->GetCurrentCoreActor())
		{
			return Core;
		}
	}
	return UGameplayStatics::GetPlayerPawn(this, 0);
}

bool ATGSwarmEnemy::GetTargetLocation(FVector& OutLocation) const
{
	if (const AActor* Target = GetTargetActor())
	{
		OutLocation = Target->GetActorLocation();
		return true;
	}
	return false;
}
