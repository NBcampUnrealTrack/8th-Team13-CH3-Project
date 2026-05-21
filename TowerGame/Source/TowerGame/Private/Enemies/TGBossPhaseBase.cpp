// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/TGBossPhaseBase.h"

#include "Enemies/TGBossBase.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"

UTGBossPhaseBase::UTGBossPhaseBase() :
	OwnerBoss(nullptr),
	PatternInterval(3.f),
	AttackSound(nullptr),
	AttackSoundVolume(0.1f),
	AttackEffect(nullptr),
	AttackEffectScale(10.0f)
{
}

void UTGBossPhaseBase::Initialize(ATGBossBase* InOwnerBoss)
{
	OwnerBoss = InOwnerBoss;
}

void UTGBossPhaseBase::EnterPhase()
{
	UE_LOG(LogTemp, Log, TEXT("%s EnterPhase"), *GetName());

	if (!OwnerBoss) return;

	TArray<FName> BreakablePartTags;

	// 공격 가능 파츠 태그
	for (FTGBossBreakablePartData& Part : BreakableParts){
		if (Part.PartTag == NAME_None || Part.HPRatio <= 0.f){
			Part.CurrentHP = 0.f;
			continue;
		}

		Part.CurrentHP = Part.HPRatio * OwnerBoss->GetMaxHP();
		BreakablePartTags.Add(Part.PartTag);
	}

	OwnerBoss->SetActiveBreakablePartTags(BreakablePartTags);

	UWorld* World = OwnerBoss->GetWorld();
	if (!World) return;

	// 패턴 공격 시작
	World->GetTimerManager().ClearTimer(PatternTimerHandle);
	World->GetTimerManager().SetTimer(
		PatternTimerHandle,
		this,
		&UTGBossPhaseBase::ExecutePattern,
		PatternInterval,
		true,
		PatternInterval
	);
}

void UTGBossPhaseBase::ExitPhase()
{
	// Phase 전환 시 이전 Phase의 반복 패턴 호출을 중단한다.
	if (OwnerBoss){
		if (UWorld* World = OwnerBoss->GetWorld()){
			World->GetTimerManager().ClearTimer(PatternTimerHandle);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("%s ExitPhase"), *GetName());
}

void UTGBossPhaseBase::ExecutePattern()
{
	UE_LOG(LogTemp, Log, TEXT("%s ExecutePattern"), *GetName());
}

FTGBossBreakablePartData* UTGBossPhaseBase::FindBreakablePart(FName PartTag)
{
	// BreakableParts에서 Tag 검색
	for (FTGBossBreakablePartData& Part : BreakableParts){
		if (Part.PartTag == PartTag){
			return &Part;
		}
	}

	return nullptr;
}

void UTGBossPhaseBase::RemoveBreakablePart(FName PartTag)
{
	BreakableParts.RemoveAll([PartTag](const FTGBossBreakablePartData& Part)
	{
		return Part.PartTag == PartTag;
	});
}

void UTGBossPhaseBase::PlayAttackSoundAtLocation(const FVector& Location) const
{
	if (!OwnerBoss || !AttackSound) return;

	UGameplayStatics::PlaySoundAtLocation(
		OwnerBoss, AttackSound, Location, AttackSoundVolume);
}

void UTGBossPhaseBase::SpawnAttackEffectAtLocation(const FVector& Location) const
{
	if (!OwnerBoss || !AttackEffect) return;

	// Effect 생성 (UParticleSystem)
	UGameplayStatics::SpawnEmitterAtLocation(
		OwnerBoss,
		AttackEffect,
		Location,
		FRotator::ZeroRotator,
		FVector(AttackEffectScale)
	);
}
