// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemies/TGMissile.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"

ATGMissile::ATGMissile()
{
	PrimaryActorTick.bCanEverTick = true;
	//	Tick 시스템에 등록은 하되 동작은 하지 않도록 함.
	//	런타임에 Tick 사용 여부를 변경하기 위함
	PrimaryActorTick.bStartWithTickEnabled = false;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	CollisionComponent->SetSphereRadius(20.f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	//	다음의 세개 채널에 대해 충돌시 폭발합니다.
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	CollisionComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	SetRootComponent(CollisionComponent);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(CollisionComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TrailEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailEffect"));
	TrailEffect->SetupAttachment(CollisionComponent);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	//	설정값 구조체를 받은 후 활성화합니다.
	ProjectileMovement->bAutoActivate = false;
	//	이동 방향을 따라서 회전합니다
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	//	종력을 비활성화합니다
	ProjectileMovement->ProjectileGravityScale = 0.f;
	//	유도기능
	ProjectileMovement->bIsHomingProjectile = false;
}

float ATGMissile::TakeDamage(
	float DamageAmount,
	const FDamageEvent& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser)
{
	const float AppliedDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (AppliedDamage <= 0.f || bHasExploded) return AppliedDamage;

	bHasExploded = true;
	OnMissileExpired.Broadcast(this);

	SetActorEnableCollision(false);
	ProjectileMovement->StopMovementImmediately();
	Destroy();

	return AppliedDamage;
}

void ATGMissile::BeginPlay()
{
	Super::BeginPlay();

	ProjectileMovement->OnProjectileStop.AddDynamic(this, &ATGMissile::OnProjectileStopped);
}

void ATGMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bUseLocationHoming || bHasExploded) return;

	// 도착 판정 — sqrt 없이 제곱 거리로 비교
	if (FVector::DistSquared(GetActorLocation(), HomingWorldLocation) <= FMath::Square(HomingArrivalRadius))
	{
		bHasExploded = true;

		FHitResult ArrivalHit;
		ArrivalHit.ImpactPoint  = HomingWorldLocation;
		ArrivalHit.ImpactNormal = FVector::UpVector;
		Explode(ArrivalHit);
		return;
	}

	const FVector ToTarget = (HomingWorldLocation - GetActorLocation()).GetSafeNormal();
	const float Speed = ProjectileMovement->Velocity.Size();

	const FQuat CurrentQuat = FRotationMatrix::MakeFromX(ProjectileMovement->Velocity.GetSafeNormal()).ToQuat();
	const FQuat TargetQuat  = FRotationMatrix::MakeFromX(ToTarget).ToQuat();
	const FQuat NewQuat     = FQuat::Slerp(CurrentQuat, TargetQuat, FMath::Clamp(HomingTurnRate * DeltaTime, 0.f, 1.f));

	ProjectileMovement->Velocity = NewQuat.GetForwardVector() * Speed;
}

void ATGMissile::LifeSpanExpired()
{
	if (!bHasExploded)
	{
		OnMissileExpired.Broadcast(this);
	}
	Super::LifeSpanExpired();
}

void ATGMissile::SetHomingTargetComponent(USceneComponent* TargetComponent)
{
	if (!TargetComponent) return;

	ProjectileMovement->bIsHomingProjectile = true;
	ProjectileMovement->HomingTargetComponent = TargetComponent;
}

void ATGMissile::SetHomingTargetActor(AActor* TargetActor)
{
	if (!TargetActor) return;
	SetHomingTargetComponent(TargetActor->GetRootComponent());
}

void ATGMissile::SetHomingLocation(const FVector& TargetLocation, float TurnRate, float ArrivalRadius)
{
	HomingWorldLocation = TargetLocation;
	HomingTurnRate = TurnRate;
	HomingArrivalRadius = ArrivalRadius;
	bUseLocationHoming = true;
	SetActorTickEnabled(true);
}

void ATGMissile::Launch(const FTGMissileParams& Params, float CollisionDisableTime)
{
	// 초기 충돌 무시 시간
	if (CollisionDisableTime > 0.f){
		CollisionComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);
		CollisionComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
		CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

		if (UWorld* World = GetWorld()){
			FTimerHandle CollisionEnableTimerHandle;
			World->GetTimerManager().SetTimer(
				CollisionEnableTimerHandle,
				FTimerDelegate::CreateWeakLambda(this, [this]()
				{
					// 초기 충돌 무시 시간 부여
					if (!CollisionComponent) return;
					CollisionComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
					CollisionComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
					CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
				}),
				CollisionDisableTime,
				false
			);
		}
	}

	ProjectileMovement->InitialSpeed = Params.InitialSpeed;
	ProjectileMovement->MaxSpeed = Params.MaxSpeed;
	ProjectileMovement->HomingAccelerationMagnitude = Params.HomingAcceleration;
	ProjectileMovement->Velocity = GetActorForwardVector() * Params.InitialSpeed;
	ProjectileMovement->Activate();

	SetLifeSpan(Params.LifeSpan);
}

void ATGMissile::OnProjectileStopped(const FHitResult& ImpactResult)
{
	if (bHasExploded) return;
	bHasExploded = true;

	Explode(ImpactResult);
}

void ATGMissile::Explode(const FHitResult& Hit)
{
	OnMissileHit.Broadcast(this, Hit);

	// 메시·충돌 즉시 제거, 이펙트 재생 후 Actor 소멸
	MeshComponent->SetHiddenInGame(true);
	SetActorEnableCollision(false);
	ProjectileMovement->StopMovementImmediately();
	Destroy();
}
