// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/TGEnemyBase.h"

#include "EngineUtils.h"
#include "BaseTower/TGBaseTower.h"
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
	const EPathFollowingRequestResult::Type MoveResult = AIController->MoveToLocation(CoreLocation);

	if (MoveResult == EPathFollowingRequestResult::Failed){
		UE_LOG(LogTemp, Warning, TEXT("Enemy MoveToLocation 실패 - 건물 공격 분기로 연결"));

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
	// 이동 성공 시 Core 공격 시작
	if (Result == EPathFollowingResult::Success){
		StartAttack(NavigationManager->GetCurrentCoreActor());
		return;
	}
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
