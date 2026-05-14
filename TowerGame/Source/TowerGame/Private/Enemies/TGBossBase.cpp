#include "Enemies/TGBossBase.h"

ATGBossBase::ATGBossBase()
{
	EnemyType = TEXT("보스");
	//	부모클래스에서 가져온 변수들
	//	임시적 사용
	//	추후 패턴별 데미지 or 패턴별 범위 지정 필요할 수도
	CurrentHP= 500.f;
	StructureAttackDamage = 20.f;
	StructureAttackRange = 250.f;
	EnergyDropAmount = 100;
}

void ATGBossBase::BeginPlay()
{
	Super::BeginPlay();

	MaxHP = CurrentHP;
	StartPattern();
}

void ATGBossBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopPattern();
	Super::EndPlay(EndPlayReason);
}

float ATGBossBase::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	const float Applied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (CurrentHP > 0.f)
	{
		CheckPhaseTransition();
	}

	return Applied;
}

void ATGBossBase::ForcePhaseChange(EBossPhase Phase)
{
	CurrentPhase = Phase;
	OnPhaseChanged(Phase);
}

//	체력 단계별 패턴 진화
void ATGBossBase::CheckPhaseTransition()
{
	if (MaxHP <= 0.f) return;

	const float HPRatio = CurrentHP / MaxHP;

	EBossPhase NewPhase = EBossPhase::Phase1;
	if (HPRatio <= Phase3HPRatio)
		NewPhase = EBossPhase::Phase3;
	else if (HPRatio <= Phase2HPRatio)
		NewPhase = EBossPhase::Phase2;

	if (NewPhase != CurrentPhase)
	{
		CurrentPhase = NewPhase;
		OnPhaseChanged(CurrentPhase);
	}
}

void ATGBossBase::OnPhaseChanged(EBossPhase NewPhase)
{
	// 행동패턴 변경
}

void ATGBossBase::ExecuteCurrentPattern()
{
	// 패턴 실행
}

void ATGBossBase::StartPattern()
{
	UWorld* World = GetWorld();
	if (!World) return;

	World->GetTimerManager().SetTimer(
		PatternTimerHandle,
		this,
		&ATGBossBase::ExecuteCurrentPattern,
		PatternInterval,
		true,
		10	//	시작전 10초 여유
	);
}

void ATGBossBase::StopPattern()
{
	UWorld* World = GetWorld();
	if (!World) return;

	World->GetTimerManager().ClearTimer(PatternTimerHandle);
}
