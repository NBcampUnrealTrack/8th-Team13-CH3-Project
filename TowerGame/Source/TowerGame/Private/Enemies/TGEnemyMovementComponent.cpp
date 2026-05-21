// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/TGEnemyMovementComponent.h"

#include "AIController.h"
#include "EngineUtils.h"
#include "NavigationSystem.h"
#include "BaseTower/TGBaseTower.h"
#include "Components/CapsuleComponent.h"
#include "Enemies/TGCoreBase.h"
#include "Enemies/TGEnemyBase.h"
#include "Enemies/TGNavigationManager.h"

// Sets default values for this component's properties
UTGEnemyMovementComponent::UTGEnemyMovementComponent() :
	OwnerEnemy(nullptr),
	NavigationManager(nullptr),
	MoveTarget(nullptr),
	GridSize(300),
	StructureAttackRange(200)
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UTGEnemyMovementComponent::Initialize(ATGEnemyBase* InOwnerEnemy, ATGNavigationManager* InNavigationManager)
{
	OwnerEnemy = InOwnerEnemy;
	NavigationManager = InNavigationManager;

	if (OwnerEnemy){
		StructureAttackRange = OwnerEnemy->GetStructureAttackRange();
	}
}

void UTGEnemyMovementComponent::RequestMoveToCore()
{
	if (!OwnerEnemy || !NavigationManager) return;

	AAIController* AIController = Cast<AAIController>(OwnerEnemy->GetController());
	if (!AIController) return;

	MoveTarget = nullptr;

	// 이동 완료 콜백 중복 바인딩 방지
	AIController->ReceiveMoveCompleted.RemoveDynamic(this, &UTGEnemyMovementComponent::HandleMoveCompleted);
	AIController->StopMovement();
	StopStructureRangeCheck();
	AIController->ReceiveMoveCompleted.AddDynamic(this, &UTGEnemyMovementComponent::HandleMoveCompleted);

	// 목적지 위치 get
	const UCapsuleComponent* CapsuleCollision = OwnerEnemy->GetCapsuleComponent();
	const float CapsuleHalfHeight = CapsuleCollision ? CapsuleCollision->GetScaledCapsuleHalfHeight() : 0.0f;
	const FVector CoreLocation = NavigationManager->GetCoreLocation()
		+ OwnerEnemy->GetNavigationHeightOffset()
		+ FVector(0, 0, CapsuleHalfHeight);

	//목적지로 이동
	EPathFollowingRequestResult::Type MoveResult = AIController->MoveToLocation(
		CoreLocation,
		StructureAttackRange,
		false,
		true,
		false,
		true,
		nullptr,
		false
	);

	if (MoveResult == EPathFollowingRequestResult::Failed){
		// 위치 보정
		if (RecoverToNavMesh()){
			UE_LOG(LogNavigation, Warning, TEXT("[Enemy:%s] 위치 보정 성공 - 보정된 위치에서 다시 RequestMoveToCore"),
			*OwnerEnemy->GetName());

			RequestMoveToCore();
			return;
		}

		return;
		// 건물 공격 - 현시점 고려하지 않음
		//MoveToBlockingBuilding();
		//return;
	}

	MoveTarget = NavigationManager->GetCurrentCoreActor();

	// 이미 목적지에 도착한 상태
	if (MoveResult == EPathFollowingRequestResult::AlreadyAtGoal){
		if (MoveTarget && IsStructureInRange(MoveTarget)){
			OnAttackRangeReached.Broadcast(MoveTarget);
		}else{
			MoveToStructureDirectly();
		}
	}
}

void UTGEnemyMovementComponent::CheckStructureRange()
{
	// 공격 목표가 지정되지 않은 경우
	if (!MoveTarget){
		StopStructureRangeCheck();
		return;
	}

	// 범위 밖
	if (!IsStructureInRange(MoveTarget)) return;

	// 이동 중지
	if (OwnerEnemy){
		if (AAIController* AIController = Cast<AAIController>(OwnerEnemy->GetController())){
			AIController->StopMovement();
		}
	}

	// 공격범위 X 이동
	StopStructureRangeCheck();
	OnAttackRangeReached.Broadcast(MoveTarget);
}

void UTGEnemyMovementComponent::StopStructureRangeCheck()
{
	if (UWorld* World = GetWorld()){
		World->GetTimerManager().ClearTimer(StructureAttackRangeCheckTimerHandle);
	}
}

void UTGEnemyMovementComponent::MoveToStructureDirectly()
{
	if (!OwnerEnemy || !MoveTarget) return;

	AAIController* AIController = Cast<AAIController>(OwnerEnemy->GetController());
	if (!AIController) return;

	const UCapsuleComponent* CapsuleCollision = OwnerEnemy->GetCapsuleComponent();
	const float CapsuleHalfHeight = CapsuleCollision ? CapsuleCollision->GetScaledCapsuleHalfHeight() : 0.0f;
	const FVector TargetLocation = MoveTarget->GetActorLocation()
		+ OwnerEnemy->GetNavigationHeightOffset()
		+ FVector(0, 0, CapsuleHalfHeight);

	// 공격 범위 보정 없이 목표 위치까지 이동
	AIController->MoveToLocation(
		TargetLocation,
		0.f,
		false,
		true,
		false,
		true,
		nullptr,
		false
	);

	UWorld* World = GetWorld();
	if (!World) return;

	// 공격 범위 안으로 들어왔는지 확인
	World->GetTimerManager().SetTimer(
		StructureAttackRangeCheckTimerHandle,
		this,
		&UTGEnemyMovementComponent::CheckStructureRange,
		0.1f,
		true
	);
}

void UTGEnemyMovementComponent::HandleMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	// 이동 성공 시 공격 시작
	if (Result == EPathFollowingResult::Success){
		if (MoveTarget && IsStructureInRange(MoveTarget)){
			OnAttackRangeReached.Broadcast(MoveTarget);
			return;
		}
		MoveToStructureDirectly();
		return;
	}else if (Result == EPathFollowingResult::Aborted){
		//if (RecoverToNavMesh()){
			RequestMoveToCore();
			return;
		//}

		return;
		// 건물 공격 - 현 시점 고려하지 않음
		//MoveToBlockingBuilding();
		//return;
	}
}

bool UTGEnemyMovementComponent::RecoverToNavMesh()
{
	UWorld* World = GetWorld();
	if (!World) return false;

	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	if (!NavSystem) return false;

	if (!OwnerEnemy || !NavigationManager) return false;

	// 보정된 위치와 거리 비교할 현재 위치
	const FVector CurrentLocation = OwnerEnemy->GetActorLocation();
	const UCapsuleComponent* CapsuleCollision = OwnerEnemy->GetCapsuleComponent();
	if (!CapsuleCollision) return false;

	// enemy 타입별 기준층에서 NavMesh를 탐색한다.
	FVector QueryLocation = CurrentLocation;
	QueryLocation.Z = NavigationManager->GetCoreLocation().Z + OwnerEnemy->GetNavigationHeightOffset().Z;

	// ProjectPointToNavigation의 결과를 저장할 변수
	FNavLocation ProjectedLocation;

	// XY는 가까운 위치를 넓게 찾고, Z는 enemy 타입별 NavMesh 층 안에서만 찾는다.
	const FVector QueryExtent(GridSize, GridSize, GridSize * 0.5f);

	// QueryExtent 범위 안에서 NavMesh 위의 위치를 탐색
	// 성공 - ProjectedLocation에 NavMesh 위 좌표 반환 / 실패 - 탐색 실패
	const bool bProjected = NavSystem->ProjectPointToNavigation(
		QueryLocation,
		ProjectedLocation,
		QueryExtent
	);

	if (!bProjected) return false;

	// Enemy의 보정 후의 좌표가 바닥과 충돌하지 않도록 HalfHeight을 합산
	ProjectedLocation.Location.Z += CapsuleCollision->GetScaledCapsuleHalfHeight();

	const float Dist = FVector::Dist(CurrentLocation, ProjectedLocation.Location);

	// 위치가 유사하면 위치 보정하지 않음
	if (Dist <= 10) return false;

	OwnerEnemy->SetActorLocation(ProjectedLocation.Location);

	return true;
}

bool UTGEnemyMovementComponent::IsStructureInRange(const AActor* Target) const
{
	if (!OwnerEnemy || !Target) return false;

	return FVector::Dist2D(OwnerEnemy->GetActorLocation(), Target->GetActorLocation()) < StructureAttackRange;
}

bool UTGEnemyMovementComponent::MoveToBlockingBuilding()
{
	MoveTarget = nullptr;

	UWorld* World = GetWorld();
	if (!World) return false;

	TArray<ABaseTower*> Buildings;

	// 모든 BaseTower을 탐색
	for (TActorIterator<ABaseTower> It(World); It; ++It){
		ABaseTower* Building = *It;
		if (!Building) continue;

		// ToDo 설치 여부 확인 - 보류
		// if(!설치 여부) continue;

		Buildings.Add(Building);
	}

	if (!OwnerEnemy) return false;

	// 거리 순으로 정렬
	Buildings.Sort([this](const ABaseTower& A, const ABaseTower& B)
	{
		return FVector::DistSquared(OwnerEnemy->GetActorLocation(), A.GetActorLocation())
			< FVector::DistSquared(OwnerEnemy->GetActorLocation(), B.GetActorLocation());
	});

	// Tower 배열을 순회하며 이동 가능한 Building을 탐색
	for (ABaseTower* Building : Buildings){
		if (!Building) continue;

		// 건물 옆칸으로 이동 여부 확인 후 가능하다면 이동
		if (TryMoveToAttackRangeOfBuilding(Building)) return true;
	}

	return false;
}

bool UTGEnemyMovementComponent::TryMoveToAttackRangeOfBuilding(ABaseTower* Building)
{
	if (!OwnerEnemy || !Building) return false;

	AAIController* AIController = Cast<AAIController>(OwnerEnemy->GetController());
	if (!AIController) return false;

	const FVector BuildingLocation = Building->GetActorLocation();

	// 목표로 이동
	const EPathFollowingRequestResult::Type MoveResult = AIController->MoveToLocation(
		BuildingLocation,
		StructureAttackRange,
		false,
		true,
		true,
		true,
		nullptr,
		false
	);

	// 이미 공격 위치에 있는 경우
	if (MoveResult == EPathFollowingRequestResult::AlreadyAtGoal){
		MoveTarget = Building;
		OnAttackRangeReached.Broadcast(MoveTarget);
		return true;
	}
	// 이동 요청 성공
	else if (MoveResult == EPathFollowingRequestResult::RequestSuccessful){
		MoveTarget = Building;
		return true;
	}

	return false;
}

