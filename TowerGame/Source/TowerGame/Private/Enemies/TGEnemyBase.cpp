// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/TGEnemyBase.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Core/GameFlow/TGGameMode.h"
#include "Enemies/TGEnemyAIController.h"
#include "Enemies/TGEnemyMovementComponent.h"

#include "Enemies/TGNavigationManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Enemy Base의 책임 과다로 이동 로직은 분리대상

// Sets default values
ATGEnemyBase::ATGEnemyBase() :
	EnemyType("Enemy"),
	MaxHP(1),
	CurrentHP(MaxHP),
	ExpDropAmount(0),
	NavigationHeightOffset(FVector::ZeroVector),
	StructureAttackDamage(0),
	StructureAttackInterval(0.5f),
	StructureAttackRange(200),
	StructureAttackSound(nullptr),
	DeathSound(nullptr),
	StructureAttackEffect(nullptr),
	EffectScale(100.0f),
	DeathEffect(nullptr),
	DeathEffectScale(1.0f)
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

	EnemyMovementComponent = CreateDefaultSubobject<UTGEnemyMovementComponent>(TEXT("EnemyMovementComponent"));
}

void ATGEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	CurrentHP = MaxHP;
	OnEnemyHpChanged.Broadcast(CurrentHP, MaxHP);

	// 스폰 경로와 무관하게(수동 배치 포함) 생존 적 목록에 자기 자신을 등록한다
	if (ATGNavigationManager* Manager = ATGNavigationManager::Get(this))
	{
		Manager->RegisterEnemy(this);
	}
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

void ATGEnemyBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 사망하지 않고 제거되는 경우(레벨 전환 등)에도 목록에서 빠지도록 보장 (Remove는 멱등)
	if (NavigationManager)
	{
		NavigationManager->UnRegisterEnemy(this);
	}
	else if (ATGNavigationManager* Manager = ATGNavigationManager::Get(this))
	{
		Manager->UnRegisterEnemy(this);
	}

	Super::EndPlay(EndPlayReason);
}

void ATGEnemyBase::InitializeEnemy(ATGNavigationManager* InNavigationManager)
{
	if (!InNavigationManager) return;

	// Enemy 등록
	NavigationManager = InNavigationManager;
	NavigationManager->RegisterEnemy(this);

	if (EnemyMovementComponent){
		EnemyMovementComponent->Initialize(this, NavigationManager);
		EnemyMovementComponent->OnAttackRangeReached.AddUniqueDynamic(
			this, &ATGEnemyBase::HandleAttackRangeReached);
	}

	// 경로 탐색
	RequestRepath();
}

void ATGEnemyBase::RequestRepath()
{
	if (!EnemyMovementComponent) return;

	StopStructureAttack();
	EnemyMovementComponent->RequestMoveToCore();
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

	// Sound 적용
	if (StructureAttackSound){
		UGameplayStatics::PlaySoundAtLocation(
			this, StructureAttackSound, GetActorLocation(), 1.f);
	}

	// Effect 생성
	if (StructureAttackEffect){
		USceneComponent* AttachComponent = FindTargetEffectAttachComponent(CurrentStructureTarget);
		if (!AttachComponent) return;

		// 이펙트 생성 위치 계산
		UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(AttachComponent);
		const FVector EffectLocation = PrimitiveComponent
			? GetRandomCollisionSurfaceLocation(PrimitiveComponent)
			: AttachComponent->GetComponentLocation();

		// NiagaraEffect 생성
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
			StructureAttackEffect,
			AttachComponent,
			NAME_None,
			EffectLocation,
			FRotator::ZeroRotator,
			EAttachLocation::KeepWorldPosition,
			true
		);

		if (NiagaraComp){
			NiagaraComp->SetWorldScale3D(FVector(EffectScale));
		}
	}
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
	// 사망 후 잔해(BodyParts) 피격 시 사망 처리·에너지 지급이 반복되지 않도록 차단
	if (bDead) return 0.f;

	float AppliedDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	CurrentHP -= AppliedDamage;
	OnEnemyHpChanged.Broadcast(CurrentHP, MaxHP);

	if (CurrentHP <= 0){
		bDead = true;
		if (ATGGameMode* GM = Cast<ATGGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
		{
			GM->AddExp(ExpDropAmount);
		}
		DestroyBodyParts();
	}

	return AppliedDamage;
}

FVector ATGEnemyBase::GetNavigationHeightOffset() const { return NavigationHeightOffset; }

float ATGEnemyBase::GetStructureAttackRange() const { return StructureAttackRange; }

FString ATGEnemyBase::GetEnemyType() const { return EnemyType; }

float ATGEnemyBase::GetCurrentHP() const { return CurrentHP; }

float ATGEnemyBase::GetMaxHP() const { return MaxHP; }

void ATGEnemyBase::HandleAttackRangeReached(AActor* TargetActor)
{
	CurrentStructureTarget = TargetActor;
	StartStructureAttack();
}

bool ATGEnemyBase::IsStructureTargetInAttackRange(const AActor* Target) const
{
	if (!Target) return false;

	return FVector::Dist2D(GetActorLocation(), Target->GetActorLocation()) < StructureAttackRange;
}

USceneComponent* ATGEnemyBase::FindTargetEffectAttachComponent(AActor* Target) const
{
	if (!Target) return nullptr;

	USceneComponent* TargetRootComponent = Target->GetRootComponent();
	if (!TargetRootComponent) return nullptr;

	// SceneRoot 바로 아래 자식만 확인한다. 그 아래 손자 Mesh들은 이펙트 대상에서 제외한다.
	for (USceneComponent* ChildComponent : TargetRootComponent->GetAttachChildren()){
		UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(ChildComponent);
		if (StaticMeshComponent && StaticMeshComponent->GetStaticMesh()){
			return StaticMeshComponent;
		}
	}

	// StaticMeshComponent가 없는 경우
	return TargetRootComponent;
}

FVector ATGEnemyBase::GetRandomCollisionSurfaceLocation(UPrimitiveComponent* Component) const
{
	if (!Component) return FVector::ZeroVector;

	// Component의 월드 기준 Bounds 정보를 가져온다.
	const FBoxSphereBounds Bounds = Component->Bounds;
	const FVector Origin = Bounds.Origin;
	const FVector Extent = Bounds.BoxExtent;

	// Bounds 박스 내부에서 임의의 월드 위치를 하나 뽑는다.
	const FVector RandomPoint(
		FMath::FRandRange(Origin.X - Extent.X, Origin.X + Extent.X),
		FMath::FRandRange(Origin.Y - Extent.Y, Origin.Y + Extent.Y),
		FMath::FRandRange(Origin.Z - Extent.Z, Origin.Z + Extent.Z)
	);

	FVector ClosestPoint = RandomPoint;
	// RandomPoint에서 가장 가까운 Collision 표면 위치를 찾는다.
	const float Distance = Component->GetClosestPointOnCollision(RandomPoint, ClosestPoint);
	// 유효한 표면 위치를 찾은 경우
	if (Distance >= 0.f){
		return ClosestPoint;
	}

	// Collision 표면 계산에 실패 -> 컴포넌트 중심 위치
	return Component->GetComponentLocation();
}

void ATGEnemyBase::DestroyBodyParts()
{
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

	// Sound 출력
	if (DeathSound){
		UGameplayStatics::PlaySoundAtLocation(
			this, DeathSound, GetActorLocation(), 1.f);
	}

	//	파괴 이펙트 출력
	if (DeathEffect){
		UGameplayStatics::SpawnEmitterAtLocation(
			this,
			DeathEffect,
			GetActorLocation(),
			GetActorRotation(),
			FVector(DeathEffectScale)
		);
	}

	SetLifeSpan(PartsLifeSpan);
}
