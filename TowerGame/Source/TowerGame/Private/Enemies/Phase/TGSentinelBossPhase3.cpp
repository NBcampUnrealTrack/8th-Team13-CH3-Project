// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Phase/TGSentinelBossPhase3.h"
#include "Enemies/Pattern/TGSphereAreaPattern.h"

UTGSentinelBossPhase3::UTGSentinelBossPhase3()
{
	// Pattern 1
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
