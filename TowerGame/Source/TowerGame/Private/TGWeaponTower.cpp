#include "TGWeaponTower.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Enemies/TGEnemyBase.h"
#include "Enemies/TGNavigationManager.h"
#include "Enemies/TGWaveManager.h"
#include "Components/SceneComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

ATGWeaponTower::ATGWeaponTower()
{
	PrimaryActorTick.bCanEverTick = true;

	MuzzlePoint1 = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzlePoint1"));
	MuzzlePoint1->SetupAttachment(WeaponMesh);

	MuzzlePoint2 = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzlePoint2"));
	MuzzlePoint2->SetupAttachment(WeaponMesh);

	//	기본적으로 인터랙션을 끕니다
	SetInteractionEnabled(false);
}

void ATGWeaponTower::BeginPlay()
{
	Super::BeginPlay();
	//	공격 로직 변경을 위해 주석처리
	//StartAttack();
	SetRangeSphereScale(AttackRange);
}

void ATGWeaponTower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AttackTarget(DeltaTime);

	//	딜레이가 끝났을 때만 적 탐지 (공격 후 쿨다운 중에는 추적 안 함)
	if (AttackPrepareTime <= DamageInterval / 2.f)
	{
		DetectingEnemy();
	}

	//	사거리 그려줄 디버그스피어
	//DrawDebugSphere(GetWorld(), GetActorLocation(), AttackRange, 16, FColor::Green);
}

void ATGWeaponTower::AttackTarget(float& DeltaTime)
{
	AttackPrepareTime -= DeltaTime;
	if (AttackPrepareTime > 0.0f)	return;
	if (Target == nullptr)	return;

	SpawnFireEffect();

	UGameplayStatics::ApplyDamage(
		Target,
		AttackDamage,
		nullptr,
		this,
		UDamageType::StaticClass()
	);
	AttackPrepareTime = DamageInterval;
	Target = nullptr;
}

void ATGWeaponTower::SpawnFireEffect()
{
	USceneComponent* Muzzle = (CurrentMuzzleIndex == 0) ? MuzzlePoint1 : MuzzlePoint2;
	CurrentMuzzleIndex = 1 - CurrentMuzzleIndex;

	if (FireEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			FireEffect,
			Muzzle->GetComponentLocation(),
			Muzzle->GetComponentRotation(),
			FVector(FireEffectScale)
		);
	}

	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Muzzle->GetComponentLocation(), FireSoundVolume);
	}
}

void ATGWeaponTower::StartAttack()
{
	// DamageInterval(초)마다 ApplyRangeDamage 무한 반복
	GetWorld()->GetTimerManager().SetTimer(
		AttackTimerHandle,
		this,
		&ATGWeaponTower::ApplyRangeDamage,
		DamageInterval,
		true
	);
}

void ATGWeaponTower::ApplyRangeDamage()
{
	FVector Center = GetActorLocation();

	// 감지 대상: Pawn 타입 (몬스터)
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	// 자기 자신은 제외
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);

	TArray<AActor*> OutActors;

	bool bHasOverlap = UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		Center,
		AttackRange,
		ObjectTypes,
		nullptr,
		IgnoreActors,
		OutActors
	);

	// 디버그용 구체 (개발 완료 후 삭제 가능)
	DrawDebugSphere(GetWorld(), Center, AttackRange, 12, FColor::Red, false, DamageInterval);

	if (bHasOverlap)
	{
		// TODO: 공격 시작 이펙트 (머즐 플래시 등) — Arrow->GetComponentLocation() / Arrow->GetComponentRotation() 기준으로 스폰
		for (AActor* HitActor : OutActors)
		{
			// 2초마다 AttackDamage만큼 데미지
			UGameplayStatics::ApplyDamage(
				HitActor,
				AttackDamage,
				nullptr,
				this,
				UDamageType::StaticClass()
			);
			// TODO: 피격 이펙트 — HitActor->GetActorLocation() 기준으로 스폰
		}
	}
}

void ATGWeaponTower::StopAttack()
{
	GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
}

void ATGWeaponTower::Upgrade()
{
	// 데미지 1.5배 증가 후 부모 호출로 UpgradeLevel++
	AttackDamage *= 1.5f;
	Super::Upgrade();
}

void ATGWeaponTower::DetectingEnemy()
{
	Target = nullptr;

	//	보스가 있으면 보스를 우선 타겟
	if (ATGWaveManager* WaveManager = ATGWaveManager::Get(this))
	{
		Target = WaveManager->GetBoss();
	}
	if (Target)	return;

	//	NavigationManager의 생존 적 목록에서 사거리 내 가장 가까운 적을 찾습니다.
	//	(매 틱 GetAllActorsOfClass 전체 스캔 제거)
	ATGNavigationManager* NavigationManager = ATGNavigationManager::Get(this);
	if (!NavigationManager)	return;

	const FVector MyLocation = GetActorLocation();
	float MinDistanceSquared = FMath::Square(AttackRange);
	for (ATGEnemyBase* Enemy : NavigationManager->GetAliveEnemies())
	{
		if (!IsValid(Enemy) || Enemy->IsDead())	continue;
		const float DistanceSquared = FVector::DistSquared(MyLocation, Enemy->GetActorLocation());
		if (DistanceSquared < MinDistanceSquared)
		{
			MinDistanceSquared = DistanceSquared;
			Target = Enemy;
		}
	}
}
