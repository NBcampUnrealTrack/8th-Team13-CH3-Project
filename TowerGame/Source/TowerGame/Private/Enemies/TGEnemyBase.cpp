// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/TGEnemyBase.h"
#include "Enemies/TGEnemyAIController.h"

#include "Enemies/TGNavigationManager.h"

#include "Navigation/PathFollowingComponent.h"

// Sets default values
ATGEnemyBase::ATGEnemyBase() : AttackDamage(1), AttackInterVal(0.5f)
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
		return;
	}

	// 이미 목적지에 도착한 상태
	if (MoveResult == EPathFollowingRequestResult::AlreadyAtGoal){
		StartCoreAttack();
	}
}

void ATGEnemyBase::SetNavigationManager(ATGNavigationManager* InNavigationManager)
{
	NavigationManager = InNavigationManager;
}

void ATGEnemyBase::StartCoreAttack()
{
	UWorld* World = GetWorld();
	if (!World) return;

	if (World->GetTimerManager().IsTimerActive(AttackTimerHandle)) return;

	// AttackInterval 마다 Core 공격
	World->GetTimerManager().SetTimer(
		AttackTimerHandle,
		this,
		&ATGEnemyBase::AttackCore,
		AttackInterVal,
		true
	);
}

void ATGEnemyBase::StopAttack()
{
	UWorld* World = GetWorld();
	if (!World) return;

	World->GetTimerManager().ClearTimer(AttackTimerHandle);
}

void ATGEnemyBase::AttackCore()
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy Core 공격 - Damage: %f"), AttackDamage);

	// TODO
	// 상위 Manager에 Core 공격 이벤트 전달
}

void ATGEnemyBase::HandleMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (Result == EPathFollowingResult::Success){
		StartCoreAttack();
		return;
	}
}


