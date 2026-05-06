#include "TGWeaponTower.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Components/StaticMeshComponent.h"

ATGWeaponTower::ATGWeaponTower()
{
	// 타이머 방식 사용으로 틱 끔 (최적화)
	PrimaryActorTick.bCanEverTick = false;

	// 무기 메시
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;
}

void ATGWeaponTower::BeginPlay()
{
	Super::BeginPlay();
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
		}
	}
}

void ATGWeaponTower::StopAttack()
{
	GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
}

void ATGWeaponTower::Upgrade()
{
	AttackDamage *= 1.5f;
	UpgradeLevel++;
}

// 플레이어 시선이 닿을 때 — 강조 표시
void ATGWeaponTower::OnFocused_Implementation(ATGPlayer* Player)
{
	if (DynamicMaterial)
	{
		DynamicMaterial->SetScalarParameterValue(TEXT("Highlight"), 1.0f);
	}
}

// 플레이어 시선이 벗어날 때 — 강조 해제
void ATGWeaponTower::OnUnfocused_Implementation(ATGPlayer* Player)
{
	if (DynamicMaterial)
	{
		DynamicMaterial->SetScalarParameterValue(TEXT("Highlight"), 0.0f);
	}
}

// 플레이어가 상호작용 키를 눌렀을 때 — 나중에 업그레이드 UI 연결
void ATGWeaponTower::OnInteract_Implementation(ATGPlayer* Player)
{
	// TODO: 업그레이드 UI 열기
}
