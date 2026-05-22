// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Phase/TGSentinelBossPhase3.h"

#include "Enemies/Pattern/TGPlayerSequenceSpherePattern.h"
#include "Enemies/Pattern/TGSphereAreaPattern.h"

UTGSentinelBossPhase3::UTGSentinelBossPhase3()
{
	// 플레이어 추적 구형 공격
	FTGBossPatternEntry SequenceSphereEntry;
	SequenceSphereEntry.PatternClass = UTGPlayerSequenceSpherePattern::StaticClass();
	SequenceSphereEntry.WarningRadius = 100.f;
	SequenceSphereEntry.AttackDamage = 20.f;
	SequenceSphereEntry.AttackEffectScale = 1.f;
	SequenceSphereEntry.PatternCooldown = 8.f;
	PatternEntries.Add(SequenceSphereEntry);

	// 플레이어 구형 공격
	FTGBossPatternEntry SphereEntry;
	SphereEntry.PatternClass = UTGSphereAreaPattern::StaticClass();
	SphereEntry.WarningRadius = 250.f;
	SphereEntry.AttackDamage = 20.f;
	SphereEntry.AttackEffectScale = 3.f;
	PatternEntries.Add(SphereEntry);

}

void UTGSentinelBossPhase3::EnterPhase()
{
	Super::EnterPhase();
	UE_LOG(LogTemp, Warning, TEXT("Sentinel Boss Phase 3 Enter"));
}

void UTGSentinelBossPhase3::ExitPhase()
{
	UE_LOG(LogTemp, Warning, TEXT("Sentinel Boss Phase 3 Exit"));
	Super::ExitPhase();
}
