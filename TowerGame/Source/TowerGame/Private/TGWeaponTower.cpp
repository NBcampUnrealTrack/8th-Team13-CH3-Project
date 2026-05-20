#include "TGWeaponTower.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Enemies/TGEnemyBase.h"
#include "Enemies/TGWaveManager.h"

ATGWeaponTower::ATGWeaponTower()
{
	PrimaryActorTick.bCanEverTick = true;

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

	//	적 탐지
	DetectingEnemy();
	AttackTarget(DeltaTime);

	//	사거리 그려줄 디버그스피어
	//DrawDebugSphere(GetWorld(), GetActorLocation(), AttackRange, 16, FColor::Green);
}

void ATGWeaponTower::AttackTarget(float& DeltaTime)
{
	AttackPrepareTime -= DeltaTime;
	if (AttackPrepareTime > 0.0f)	return;
	if (Target == nullptr)	return;

	UGameplayStatics::ApplyDamage(
		Target,
		AttackDamage,
		nullptr,
		this,
		UDamageType::StaticClass()
	);
	AttackPrepareTime = DamageInterval;
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
	//	가장 가까운 적을 찾습니다.
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATGEnemyBase::StaticClass(), FoundActors);

	float MinDistance = 500000.f;
	Target = nullptr;
	Target = ATGWaveManager::Get(this)->GetBoss();
	if (Target)	return;

	for (AActor* Actor : FoundActors)
	{
		ATGEnemyBase* Enemy = Cast<ATGEnemyBase>(Actor);
		if (Enemy->GetCurrentHP() <= 0)	continue;
		FVector MyLocation = GetActorLocation();
		FVector TargetLocation = Actor->GetActorLocation();
		float Distance = FVector::Distance(MyLocation, TargetLocation);
		if (Distance > AttackRange)	continue;
		if (Distance < MinDistance)
		{
			MinDistance = Distance;
			Target = Actor;
		}
	}
}
