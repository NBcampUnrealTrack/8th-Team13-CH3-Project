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
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Navigation/PathFollowingComponent.h"

// Sets default values
ATGEnemyBase::ATGEnemyBase() :
	EnergyDropAmount(0),
	EnemyType("Enemy"),
	StructureAttackDamage(0),
	StructureAttackInterval(0.5f),
	StructureAttackRange(200),
	MaxHP(1),
	CurrentHP(MaxHP),
	NavigationHeightOffset(FVector::ZeroVector),
	GridSize(300)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	AIControllerClass = ATGEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Enemy 회피 기능
	GetCharacterMovement()->bUseRVOAvoidance = true;
	GetCharacterMovement()->AvoidanceConsiderationRadius = 50;

	// 충돌 무시
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
}

void ATGEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	CurrentHP = MaxHP;
	OnEnemyHpChanged.Broadcast(CurrentHP, MaxHP);
	//	하위 스태틱 메쉬 컴포넌트 모두 가져오기
	TArray<UStaticMeshComponent*> Components;
	GetComponents<UStaticMeshComponent>(Components);
	for (UStaticMeshComponent* Component : Components)
	{
		if (Component == RootComponent || !Component->GetStaticMesh())
		{
			continue;
		}

		//Component->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Component->SetCanEverAffectNavigation(false);
		Component->SetSimulatePhysics(false);
		BodyParts.Add(Component);
	}
}

void ATGEnemyBase::InitializeEnemy(ATGNavigationManager* InNavigationManager)
{
	if (!InNavigationManager) return;

	// Enemy 등록
	NavigationManager = InNavigationManager;
	NavigationManager->RegisterEnemy(this);

	// 경로 탐색
	RequestRepath();
}

FVector ATGEnemyBase::GetNavigationHeightOffset() const
{
	return NavigationHeightOffset;
}

void ATGEnemyBase::RequestRepath()
{
	if (!NavigationManager) return;

	AAIController* AIController = Cast<AAIController>(GetController());
	if (!AIController) return;

	// 이동 완료 콜백 중복 바인딩 방지
	AIController->ReceiveMoveCompleted.RemoveDynamic(this, &ATGEnemyBase::HandleMoveCompleted);
	AIController->StopMovement();
	StopStructureAttack();
	StopStructureAttackRangeCheck();
	AIController->ReceiveMoveCompleted.AddDynamic(this, &ATGEnemyBase::HandleMoveCompleted);

	// 목적지 위치 get
	const UCapsuleComponent* CapsuleCollision = GetCapsuleComponent();
	const float CapsuleHalfHeight = CapsuleCollision ? CapsuleCollision->GetScaledCapsuleHalfHeight() : 0.0f;
	const FVector CoreLocation = NavigationManager->GetCoreLocation()
		+ NavigationHeightOffset
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
		if (TryRecoverToNearestNavMesh()){
			UE_LOG(LogNavigation, Warning, TEXT("[Enemy:%s] 위치 보정 성공 - 보정된 위치에서 다시 RequestRepath"),
			*GetName());

			RequestRepath();
			return;
		}

		return;
		// 건물 공격 - 현시점 고려하지 않음
		//MoveToBlockingBuilding();
		//return;
	}

	CurrentStructureTarget = NavigationManager->GetCurrentCoreActor();

	// 이미 목적지에 도착한 상태
	if (MoveResult == EPathFollowingRequestResult::AlreadyAtGoal){
		if (CurrentStructureTarget && IsStructureTargetInAttackRange(CurrentStructureTarget)){
			StartStructureAttack();
		}else{
			MoveDirectlyToStructureTarget();
		}
	}
}

void ATGEnemyBase::SetNavigationManager(ATGNavigationManager* InNavigationManager)
{
	NavigationManager = InNavigationManager;
}

void ATGEnemyBase::StartStructureAttack()
{
	if (!CurrentStructureTarget) return;

	UWorld* World = GetWorld();
	if (!World) return;

	World->GetTimerManager().ClearTimer(StructureAttackTimerHandle);

	// StructureAttackInterval 마다 Core 공격
	World->GetTimerManager().SetTimer(
		StructureAttackTimerHandle,
		this,
		&ATGEnemyBase::AttackStructureTarget,
		StructureAttackInterval,
		true
	);
}

void ATGEnemyBase::StopStructureAttackRangeCheck()
{
	if (UWorld* World = GetWorld()){
		World->GetTimerManager().ClearTimer(StructureAttackRangeCheckTimerHandle);
	}
}

void ATGEnemyBase::CheckStructureAttackRange()
{
	// 공격 목표가 지정되지 않은 경우
	if (!CurrentStructureTarget){
		StopStructureAttackRangeCheck();
		return;
	}

	// 범위 밖
	if (!IsStructureTargetInAttackRange(CurrentStructureTarget)) return;

	// 이동 중지
	if (AAIController* AIController = Cast<AAIController>(GetController())){
		AIController->StopMovement();
	}

	// 공격범위 X 이동
	StopStructureAttackRangeCheck();
	StartStructureAttack();
}

void ATGEnemyBase::MoveDirectlyToStructureTarget()
{
	if (!CurrentStructureTarget) return;

	AAIController* AIController = Cast<AAIController>(GetController());
	if (!AIController) return;

	const UCapsuleComponent* CapsuleCollision = GetCapsuleComponent();
	const float CapsuleHalfHeight = CapsuleCollision ? CapsuleCollision->GetScaledCapsuleHalfHeight() : 0.0f;
	const FVector TargetLocation = CurrentStructureTarget->GetActorLocation()
		+ NavigationHeightOffset
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
		&ATGEnemyBase::CheckStructureAttackRange,
		0.1f,
		true
	);
}

void ATGEnemyBase::StopStructureAttack()
{
	UWorld* World = GetWorld();
	if (!World) return;

	World->GetTimerManager().ClearTimer(StructureAttackTimerHandle);
	CurrentStructureTarget = nullptr;
}

float ATGEnemyBase::TakeDamage(float DamageAmount,
	const FDamageEvent& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser)
{
	float AppliedDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	CurrentHP -= AppliedDamage;
	OnEnemyHpChanged.Broadcast(CurrentHP, MaxHP);

	if (CurrentHP <= 0){
		if (ATGGameMode* GM = Cast<ATGGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
		{
			GM->AddEnergy(EnergyDropAmount);
		}
		DestroyUnit();
	}

	return AppliedDamage;
}

FString ATGEnemyBase::GetEnemyType()
{
	return EnemyType;
}

float ATGEnemyBase::GetCurrentHP()
{
	return CurrentHP;
}

float ATGEnemyBase::GetMaxHP()
{
	return MaxHP;
}

void ATGEnemyBase::AttackStructureTarget()
{
	// 공격 목표가 없는 경우 중단
	if (!CurrentStructureTarget){
		StopStructureAttack();
		return;
	}

	// 공격 목표가 공격 범위 밖인 경우 공격 중지 및 재탐색
	if (!IsStructureTargetInAttackRange(CurrentStructureTarget)){
		StopStructureAttack();
		RequestRepath();
		return;
	}

	// Target(CoreBase / BaseTower) 공격
	UGameplayStatics::ApplyDamage(
		CurrentStructureTarget,
		StructureAttackDamage,
		GetController(),
		this,
		nullptr
	);
}

void ATGEnemyBase::HandleMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	// 이동 성공 시 공격 시작
	if (Result == EPathFollowingResult::Success){
		if (CurrentStructureTarget && IsStructureTargetInAttackRange(CurrentStructureTarget)){
			StartStructureAttack();
			return;
		}
		MoveDirectlyToStructureTarget();
		return;
	}else if (Result == EPathFollowingResult::Aborted){
		//if (TryRecoverToNearestNavMesh()){
			RequestRepath();
			return;
		//}

		return;
		// 건물 공격 - 현 시점 고려하지 않음
		//MoveToBlockingBuilding();
		//return;
	}
}

bool ATGEnemyBase::TryRecoverToNearestNavMesh()
{
	UWorld* World = GetWorld();
	if (!World) return false;

	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	if (!NavSystem) return false;

	// 보정된 위치와 거리 비교할 현재 위치
	const FVector CurrentLocation = GetActorLocation();
	const UCapsuleComponent* CapsuleCollision = GetCapsuleComponent();
	if (!CapsuleCollision) return false;

	// enemy 타입별 기준층에서 NavMesh를 탐색한다.
	FVector QueryLocation = CurrentLocation;
	QueryLocation.Z = NavigationManager->GetCoreLocation().Z + NavigationHeightOffset.Z;

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

	SetActorLocation(ProjectedLocation.Location);

	return true;
}

bool ATGEnemyBase::IsStructureTargetInAttackRange(const AActor* Target) const
{
	if (!Target) return false;

	return FVector::Dist2D(GetActorLocation(), Target->GetActorLocation()) < StructureAttackRange;
}

bool ATGEnemyBase::MoveToBlockingBuilding()
{
	CurrentStructureTarget = nullptr;

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

	// 거리 순으로 정렬
	Buildings.Sort([this](const ABaseTower& A, const ABaseTower& B)
	{
		return FVector::DistSquared(GetActorLocation(), A.GetActorLocation())
			< FVector::DistSquared(GetActorLocation(), B.GetActorLocation());
	});

	// Tower 배열을 순회하며 이동 가능한 Building을 탐색
	for (ABaseTower* Building : Buildings){
		if (!Building) continue;

		// 건물 옆칸으로 이동 여부 확인 후 가능하다면 이동
		if (TryMoveToAttackRangeOfBuilding(Building)) return true;
	}

	return false;
}

bool ATGEnemyBase::TryMoveToAttackRangeOfBuilding(ABaseTower* Building)
{
	AAIController* AIController = Cast<AAIController>(GetController());
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
		CurrentStructureTarget = Building;
		StartStructureAttack();
		return true;
	}
	// 이동 요청 성공
	else if (MoveResult == EPathFollowingRequestResult::RequestSuccessful){
		CurrentStructureTarget = Building;
		return true;
	}

	return false;
}

void ATGEnemyBase::DestroyUnit()
{	//	사망을 이 함수로 대체해야합니다.
	StopStructureAttack();

	// NavigationManger 등록 해제
	if (NavigationManager)
	{
		NavigationManager->UnRegisterEnemy(this);
	}

	OnEnemyRemoved.Broadcast(this);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	for (UStaticMeshComponent* BodyPart : BodyParts)
	{
		BodyPart->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		BodyPart->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		BodyPart->SetSimulatePhysics(true);
		BodyPart->AddRadialImpulse(GetActorLocation(), ExplodeRadius, ExplodeForce, RIF_Linear, true);
		BodyPart->AddTorqueInDegrees(
			FVector(FMath::FRandRange(-180.f,180.f),
				FMath::FRandRange(-180.f,180.f),
				FMath::FRandRange(-180.f,180.f))
			);
	}

	//	파괴 이펙트 출력
	//	파괴 사운드 출력

	SetLifeSpan(5.0f);
}
