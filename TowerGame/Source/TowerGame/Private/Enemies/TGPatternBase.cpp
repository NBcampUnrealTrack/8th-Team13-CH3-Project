// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/TGPatternBase.h"

#include "TGMountedTower.h"
#include "Enemies/TGBossBase.h"
#include "Enemies/TGBossPhaseBase.h"
#include "Enemies/TGCoreBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/TGPlayer.h"

UTGPatternBase::UTGPatternBase() :
	OwnerPhase(nullptr),
	OwnerBoss(nullptr),
	WarningRadius(0.f),
	AttackDamage(0.f),
	AttackLocation(FVector::ZeroVector),
	AttackSound(nullptr),
	AttackSoundVolume(1.f),
	AttackEffect(nullptr),
	AttackEffectScale(1.f)
{
}

void UTGPatternBase::Initialize(
	UTGBossPhaseBase* InOwnerPhase,
	ATGBossBase* InOwnerBoss,
	float InWarningRadius,
	float InAttackDamage,
	float InAttackEffectScale)
{
	OwnerPhase = InOwnerPhase;
	OwnerBoss = InOwnerBoss;
	WarningRadius = InWarningRadius;
	AttackDamage = InAttackDamage;
	AttackEffectScale = InAttackEffectScale;
}

void UTGPatternBase::GetAttackLocation()
{
	AttackLocation = FVector::ZeroVector;
}

void UTGPatternBase::DrawWarning(float WarningDrawTime)
{
}

void UTGPatternBase::ExecuteAttack()
{
	TArray<AActor*> DamageTargets;
	// 공격 대상 탐색
	CollectDamageTargets(DamageTargets);

	// Sound / Effect
	PlayAttackFeedback(AttackLocation);

	// 공격 대상에게 데미지 적용
	ApplyDamageToTargets(DamageTargets, AttackDamage);
}

void UTGPatternBase::CollectDamageTargets(TArray<AActor*>& OutTargets) const
{
}

bool UTGPatternBase::PrepareOverlapQuery(
	UWorld*& OutWorld,
	TArray<TEnumAsByte<EObjectTypeQuery>>& OutObjectTypes,
	TArray<AActor*>& OutIgnoreActors) const
{
	if (!OwnerBoss) return false;

	OutWorld = OwnerBoss->GetWorld();
	if (!OutWorld) return false;

	// 탐색 대상 설정
	OutIgnoreActors.Add(OwnerBoss);

	OutObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	OutObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

	return true;
}

bool UTGPatternBase::IsValidDamageTarget(AActor* TargetActor) const
{
	if (!TargetActor) return false;

	return Cast<ATGPlayer>(TargetActor) || Cast<ATGCoreBase>(TargetActor) || Cast<ATGMountedTower>(TargetActor);
}

void UTGPatternBase::PlayAttackFeedback(const FVector& Location) const
{
	if (!OwnerBoss) return;

	UWorld* World = OwnerBoss->GetWorld();
	if (!World) return;

	// 사운드 출력
	if (AttackSound){
		UGameplayStatics::PlaySoundAtLocation(
			World,
			AttackSound,
			Location,
			AttackSoundVolume
		);
	}

	// 이펙트 출력
	if (AttackEffect){
		UGameplayStatics::SpawnEmitterAtLocation(
			World,
			AttackEffect,
			Location,
			FRotator::ZeroRotator,
			FVector(AttackEffectScale)
		);
	}
}

void UTGPatternBase::ApplyDamageToTargets(const TArray<AActor*>& Targets, float Damage) const
{
	if (!OwnerBoss) return;

	for (AActor* Target : Targets){
		if (!IsValidDamageTarget(Target)) continue;

		UGameplayStatics::ApplyDamage(
			Target,
			Damage,
			nullptr,
			OwnerBoss,
			nullptr
		);
	}
}
