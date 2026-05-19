// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/TGBossBase.h"

#include "TGMountedTower.h"
#include "Core/GameFlow/TGGameMode.h"
#include "Enemies/TGBossPhaseBase.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Player/TGPlayer.h"

ATGBossBase::ATGBossBase() :
	MaxHP(1.f),
	CurrentHP(0.f),
	TowerDamageMultiplier(0.1f),
	SpawnClearRadius(0),
	CurrentPhase(nullptr),
	CurrentPhaseIndex(INDEX_NONE),
	PartsLifeSpan(3.f),
	ExplodeRadius(100.f),
	ExplodeForce(50.f),
	TargetPlayer(nullptr)
{
	PrimaryActorTick.bCanEverTick = false;

	BossName = FText::FromString("Boss");

}

void ATGBossBase::BeginPlay()
{
	Super::BeginPlay();

	CurrentHP = MaxHP;
	OnBossHpChanged.Broadcast(CurrentHP, MaxHP);
	TargetPlayer = Cast<ATGPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));

	ChangeToNextPhase();
}

void ATGBossBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (CurrentPhase){
		CurrentPhase->ExitPhase();
		CurrentPhase = nullptr;
	}
	OnBossRemoved.Broadcast(this);

	Super::EndPlay(EndPlayReason);
}

float ATGBossBase::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	float AppliedDamage =  Super::TakeDamage(
		DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// Tower 데미지는 보정 후 적용
	if (Cast<ATGMountedTower>(DamageCauser)){
		AppliedDamage *= TowerDamageMultiplier;
	}

	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID)){
		const FPointDamageEvent& PointDamageEvent = static_cast<const FPointDamageEvent&>(DamageEvent);

		const float PartDamageResult =
			ApplyBreakablePartDamage(PointDamageEvent.HitInfo.GetComponent(), AppliedDamage);

		if (PartDamageResult >= 0.f){
			AppliedDamage = PartDamageResult;
		}
	}

	// 데미지 적용 후 페이즈 전환 검사
	ApplyBossDamage(AppliedDamage);
	CheckPhaseTransition();

	return AppliedDamage;
}

FText ATGBossBase::GetBossName() const
{
	return BossName;
}

float ATGBossBase::GetCurrentHP() const
{
	return CurrentHP;
}

float ATGBossBase::GetMaxHP() const
{
	return MaxHP;
}

float ATGBossBase::GetCurrentPhaseMinHP() const
{
	if (!PhaseHPRatio.IsValidIndex(CurrentPhaseIndex)) return 0.f;

	return PhaseHPRatio[CurrentPhaseIndex] * MaxHP;
}

float ATGBossBase::GetCurrentPhaseMaxHP() const
{
	if (CurrentPhaseIndex <= 0) return MaxHP;

	const int32 PreviousPhaseIndex = CurrentPhaseIndex - 1;
	if (!PhaseHPRatio.IsValidIndex(PreviousPhaseIndex)) return MaxHP;

	return PhaseHPRatio[PreviousPhaseIndex] * MaxHP;
}

float ATGBossBase::GetSpawnClearRadius() const
{
	return SpawnClearRadius;
}

ATGPlayer* ATGBossBase::GetPlayer() const
{
	return TargetPlayer;
}

void ATGBossBase::SetActiveBreakablePartTags(const TArray<FName>& InBreakablePartTags)
{
	ActiveBreakablePartTags.Empty();

	// Set에 Tag 추가 공격 가능 파츠 탐색용
	for (const FName& PartTag : InBreakablePartTags){
		if (PartTag != NAME_None){
			ActiveBreakablePartTags.Add(PartTag);
		}
	}
}

void ATGBossBase::ChangeToNextPhase()
{
	const int32 NewPhaseIndex = CurrentPhaseIndex + 1;
	if (!PhaseClasses.IsValidIndex(NewPhaseIndex)) return;

	TSubclassOf<UTGBossPhaseBase> NewPhaseClass = PhaseClasses[NewPhaseIndex];
	if (!NewPhaseClass) return;

	// 기존 페이즈 종료
	if (CurrentPhase){
		DestroyDetachedPartComponent();

		CurrentPhase->ExitPhase();
		CurrentPhase = nullptr;
	}

	// 페이즈 생성
	UTGBossPhaseBase* NewPhase = NewObject<UTGBossPhaseBase>(this, NewPhaseClass);
	if (!NewPhase) return;

	// Phase에 Boss 참조
	NewPhase->Initialize(this);
	CurrentPhase = NewPhase;
	CurrentPhaseIndex = NewPhaseIndex;

	CurrentPhase->EnterPhase();
}

void ATGBossBase::CheckPhaseTransition()
{
	if (GetWorldTimerManager().IsTimerActive(PhaseTransitionTimerHandle)) return;

	const int32 NextPhaseIndex = CurrentPhaseIndex + 1;
	if (!PhaseClasses.IsValidIndex(NextPhaseIndex) || !PhaseHPRatio.IsValidIndex(CurrentPhaseIndex)) return;

	// HP 비율이 현재 페이즈의 전환 기준 이하로 내려가면 다음 페이즈로 전환
	const float HPRatio = CurrentHP / MaxHP;
	if (HPRatio > PhaseHPRatio[CurrentPhaseIndex]) return;

	GetWorldTimerManager().SetTimer(
		PhaseTransitionTimerHandle, this, &ATGBossBase::ChangeToNextPhase, 1, false);
}

void ATGBossBase::ApplyBossDamage(float DamageAmount)
{
	if (DamageAmount <= 0.f || CurrentHP <= 0.f) return;

	float MinHpRatio = 0.f;
	if (PhaseHPRatio.IsValidIndex(CurrentPhaseIndex)) MinHpRatio = PhaseHPRatio[CurrentPhaseIndex];

	CurrentHP = FMath::Clamp(CurrentHP - DamageAmount, MinHpRatio * MaxHP, MaxHP);
	OnBossHpChanged.Broadcast(CurrentHP, MaxHP);

	UE_LOG(LogTemp, Warning, TEXT("[BossBase]BossDamaged - Damage: %.1f / HP: %.1f / %.1f "),
		DamageAmount, CurrentHP, MaxHP);

	if (CurrentHP <= 0){
		if (ATGGameMode* GameMode = Cast<ATGGameMode>(UGameplayStatics::GetGameMode(this))){
			GameMode->HandleGameClear();
		}
		Destroy();
	}
}

float ATGBossBase::ApplyBreakablePartDamage(UActorComponent* HitComponent, float DamageAmount)
{
	if (!HitComponent || !CurrentPhase || DamageAmount <= 0.f) return -1.f;

	for (const FName& ComponentTag : HitComponent->ComponentTags){
		if (!ActiveBreakablePartTags.Contains(ComponentTag)) continue;

		// 해당 페이즈에 해당 파츠가 존재하는지 확인
		FTGBossBreakablePartData* PartData = CurrentPhase->FindBreakablePart(ComponentTag);
		if (!PartData){
			ActiveBreakablePartTags.Remove(ComponentTag);
			return -1.f;
		}

		// Tag 존재 + 체력 0 -> 파괴 중인 파츠
		if (PartData->CurrentHP <= 0){
			// 파괴 처리 함수 구현 후 Tag 제거 시점 변경 예정
			ActiveBreakablePartTags.Remove(ComponentTag);
			CurrentPhase->RemoveBreakablePart(ComponentTag);
			return 0.f;
		}

		// 파츠에 체력에 데미지를 보정해서 적용
		float AppliedPartDamage = FMath::Clamp(DamageAmount, 0.f, PartData->CurrentHP);
		PartData->CurrentHP -= AppliedPartDamage;

		UE_LOG(LogTemp, Warning, TEXT("[BossPart] Damaged - Part: %s / Damage: %.1f / HP: %.1f"),
					*ComponentTag.ToString(), AppliedPartDamage, PartData->CurrentHP);

		// 파츠의 체력이 0일 경우 파괴 추가 데미지 적용
		if (PartData->CurrentHP <= 0){
			AppliedPartDamage += MaxHP * PartData->DestroyBonusDamageRatio;

			UE_LOG(LogTemp, Warning, TEXT("[BossPart] Destroyed - Part: %s / Damage: %.1f"),
				*ComponentTag.ToString(), AppliedPartDamage);

			DestroyBreakableParts(ComponentTag);
		}

		return AppliedPartDamage;
	}
	return -1.f;
}

void ATGBossBase::DestroyBreakableParts(FName PartTag)
{
	// 보스의 하위 StaticMeshCompoenet 전체 수집
	TArray<UStaticMeshComponent*> StaticMeshComponents;
	GetComponents<UStaticMeshComponent>(StaticMeshComponents);

	for (UStaticMeshComponent* StaticMesh: StaticMeshComponents){
		// 태그가 일치하는 컴포넌트만 필터링
		if (!StaticMesh || !StaticMesh->ComponentHasTag(PartTag)) continue;

		// 부모로부터 분리 및 충돌/네비/물리 설정
		StaticMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		StaticMesh->SetCanEverAffectNavigation(false);
		StaticMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		StaticMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		StaticMesh->SetSimulatePhysics(true);

		// 폭발 적용(Force)
		StaticMesh->AddRadialForce(
			GetActorLocation(),
			ExplodeRadius,
			ExplodeForce,
			RIF_Linear,
			true
		);

		// 폭발 적용(Rotation)
		StaticMesh->AddTorqueInDegrees(FVector(
			FMath::FRandRange(-180.f,180.f),
			FMath::FRandRange(-180.f,180.f),
			FMath::FRandRange(-180.f,180.f)
		));

		FTimerHandle DestroyPartTimerHandle;
		FTimerDelegate DestroyPartDelegate;
		DestroyPartDelegate.BindUObject(
			this,
			&ATGBossBase::DestroyDetachedPartComponent,
			StaticMesh,
			PartTag
		);

		// 파괴된 파츠가 Level에서 사라질 때 호출됨
		GetWorldTimerManager().SetTimer(
			DestroyPartTimerHandle,
			DestroyPartDelegate,
			PartsLifeSpan,
			false
		);
	}
}

void ATGBossBase::DestroyDetachedPartComponent(UStaticMeshComponent* StaticMesh, FName PartTag)
{
	if (StaticMesh && !StaticMesh->IsBeingDestroyed()){
		StaticMesh->DestroyComponent();
	}

	// Tag 제거
	ActiveBreakablePartTags.Remove(PartTag);
	if (CurrentPhase){
		CurrentPhase->RemoveBreakablePart(PartTag);
	}
}

void ATGBossBase::DestroyDetachedPartComponent()
{
	if (!CurrentPhase) return;

	// Set -> Array
	TArray<FName> PartTags = ActiveBreakablePartTags.Array();


	for (const FName& PartTag : PartTags){
		FTGBossBreakablePartData* PartData = CurrentPhase->FindBreakablePart(PartTag);
		// PartData가 없거나 현재 체력이 없는 얘들은 이미 부위 파괴 처리 중인것으로 간주
		if (!PartData || PartData->CurrentHP <= 0.f) continue;

		DestroyBreakableParts(PartTag);
	}
}
