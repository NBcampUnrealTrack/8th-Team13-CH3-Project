// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/TGEnemyBase.h"

#include "EngineUtils.h"
#include "NavigationSystem.h"
#include "BaseTower/TGBaseTower.h"
#include "Components/CapsuleComponent.h"
#include "Core/GameFlow/TGGameMode.h"
#include "Enemies/TGCoreBase.h"
#include "Enemies/TGEnemyAIController.h"

#include "Enemies/TGNavigationManager.h"
#include "Kismet/GameplayStatics.h"

#include "Navigation/PathFollowingComponent.h"

// Sets default values
ATGEnemyBase::ATGEnemyBase() : AttackDamage(1), AttackInterVal(0.5f), HP(10)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	AIControllerClass = ATGEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ATGEnemyBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopAttack();

	// NavigationManger 등록 해제
	if (NavigationManager)
	{
		NavigationManager->UnRegisterEnemy(this);
	}

	Super::EndPlay(EndPlayReason);
}

void ATGEnemyBase::InitializeEnemy(ATGNavigationManager* InNavigationManager)
{
	if (!InNavigationManager) return;

	// Enemy 등록
	UE_LOG(LogTemp, Warning, TEXT("네비게이션 등록"));
	NavigationManager = InNavigationManager;
	NavigationManager->RegisterEnemy(this);

	// 경로 탐색
	RequestRepath();
}

void ATGEnemyBase::RequestRepath()
{
	if (!NavigationManager) return;

	AAIController* AIController = Cast<AAIController>(GetController());
	if (!AIController) return;

	StopAttack();

	// 이동 완료 콜백 중복 바인딩 방지
	AIController->ReceiveMoveCompleted.RemoveDynamic(this, &ATGEnemyBase::HandleMoveCompleted);
	AIController->ReceiveMoveCompleted.AddDynamic(this, &ATGEnemyBase::HandleMoveCompleted);

	// 목적지 위치 get
	const FVector CoreLocation = NavigationManager->GetCoreLocation();

	//목적지로 이동
	EPathFollowingRequestResult::Type MoveResult = AIController->MoveToLocation(CoreLocation);

	if (MoveResult == EPathFollowingRequestResult::Failed){
		UE_LOG(LogTemp, Warning, TEXT("Enemy MoveToLocation 실패 &  NaviMesh 위에 존재 - 건물 공격 분기로 연결"));

		// 건물 공격
		AActor* BlockingBuilding = FindBlockingBuilding();
		StartAttack(BlockingBuilding);
		return;
	}

	// 이미 목적지에 도착한 상태
	if (MoveResult == EPathFollowingRequestResult::AlreadyAtGoal){
		StartAttack(NavigationManager->GetCurrentCoreActor());
	}
}

void ATGEnemyBase::SetNavigationManager(ATGNavigationManager* InNavigationManager)
{
	NavigationManager = InNavigationManager;
}

void ATGEnemyBase::StartAttack(AActor* TargetActor)
{
	if (!TargetActor) return;

	UWorld* World = GetWorld();
	if (!World) return;

	StopAttack();

	CurrentAttackTarget = TargetActor;

	if (World->GetTimerManager().IsTimerActive(AttackTimerHandle)) return;

	// AttackInterval 마다 Core 공격
	World->GetTimerManager().SetTimer(
		AttackTimerHandle,
		this,
		&ATGEnemyBase::AttackTarget,
		AttackInterVal,
		true
	);
}

void ATGEnemyBase::StopAttack()
{
	UWorld* World = GetWorld();
	if (!World) return;

	World->GetTimerManager().ClearTimer(AttackTimerHandle);
	CurrentAttackTarget = nullptr;
}

float ATGEnemyBase::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	float AppliedDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	HP -= AppliedDamage;

	UE_LOG(LogTemp, Warning, TEXT("Enemy 피격 - Damage: %.1f / HP: %.1f"), AppliedDamage, HP);

	if (HP <= 0){
		if (ATGGameMode* GM = Cast<ATGGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
		{
			GM->AddEnergy(EnergyDropAmount);
		}
		Destroy();
	}

	return AppliedDamage;
}

void ATGEnemyBase::AttackTarget()
{
	if (!CurrentAttackTarget){
		StopAttack();
		return;
	}

	UE_LOG(LogTemp, Warning,
		TEXT("Enemy 공격 - Target: %s /  Damage: %f"), *CurrentAttackTarget->GetName(),AttackDamage);

	// Target(CoreBase / BaseTower) 공격
	UGameplayStatics::ApplyDamage(
		CurrentAttackTarget,
		AttackDamage,
		GetController(),
		this,
		nullptr
	);
}

void ATGEnemyBase::HandleMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	UE_LOG(LogTemp, Error, TEXT("EPathFollowingResult: %d"), Result);

	// 이동 성공 시 Core 공격 시작
	if (Result == EPathFollowingResult::Success){
		StartAttack(NavigationManager->GetCurrentCoreActor());
		return;
	}else if (Result == EPathFollowingResult::Aborted){
		UE_LOG(LogTemp, Warning, TEXT("이동 도중 중단됨"));

		if (TryRecoverToNearestNavMesh()){
			RequestRepath();
			return;
		}

		AActor* BlockingBuilding = FindBlockingBuilding();
		StartAttack(BlockingBuilding);
		return;
	}
}

bool ATGEnemyBase::TryRecoverToNearestNavMesh()
{
	UWorld* World = GetWorld();
	if (!World) return false;

	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	if (!NavSystem) return false;

	// ProjectPointToNavigation의 결과를 저장할 변수
	FNavLocation ProjectedLocation;
	const FVector CurrentLocation = GetActorLocation();

	// 검색 범위 (격자 크기)
	const FVector QueryExtent(300, 300, 300);

	// QueryExtent 범위 안에서 NavMesh 위의 위치를 탐색
	// 성공 - ProjectedLocation에 NevMesh 위 좌표 반환 / 실패 - 탐색 실패
	const bool bProjected = NavSystem->ProjectPointToNavigation(
		CurrentLocation,
		ProjectedLocation,
		QueryExtent
	);

	if (!bProjected) return false;

	const UCapsuleComponent* CapsuleCollision = GetCapsuleComponent();
	if (!CapsuleCollision) return false;

	// Enemy의 보정 후의 좌표가 바닥과 충돌하지 않도록 HalfHeight을 합산
	ProjectedLocation.Location.Z += CapsuleCollision->GetScaledCapsuleHalfHeight();

	const float Dist2D = FVector::Dist2D(CurrentLocation, ProjectedLocation.Location);

	// 위치가 유사하면 위치 보정하지 않음
	if (Dist2D <= 10) return false;

	SetActorLocation(ProjectedLocation.Location);

	return true;
}

AActor* ATGEnemyBase::FindBlockingBuilding() const
{
	UWorld* World = GetWorld();
	if (!World) return nullptr;

	AAIController* AIController = Cast<AAIController>(GetController());
	if (!AIController) return nullptr;

	TArray<ABaseTower*> Buildings;

	// 모든 BaseTower을 탐색
	for (TActorIterator<ABaseTower> It(World); It; ++It){
		if (*It) Buildings.Add(*It);
	}

	// 거리 순으로 정렬
	Buildings.Sort([this](const ABaseTower& A, const ABaseTower& B)
	{
		return FVector::DistSquared(GetActorLocation(), A.GetActorLocation())
			< FVector::DistSquared(GetActorLocation(), B.GetActorLocation());
	});

	// Tower 배열을 순회하며 이동 가능한 Building을 탐색
	for (ABaseTower* Building : Buildings){
		if (!Building) continue;

		const EPathFollowingRequestResult::Type MoveResult =
			AIController->MoveToLocation(Building->GetActorLocation());

		if (MoveResult != EPathFollowingRequestResult::Failed) return Building;
	}

	return nullptr;
}
