// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Phase/TGSentinelBossPhase2.h"

#include "Enemies/Pattern/TGRandomGroundCylinderPattern.h"
#include "Enemies/Pattern/TGSphereAreaPattern.h"

UTGSentinelBossPhase2::UTGSentinelBossPhase2()
{
	// Phase 파괴 가능 Part 설정
	FTGBossBreakablePartData BodyPart;
	BodyPart.PartTag = TEXT("Body");
	BodyPart.HPRatio = 0.15f;
	BodyPart.DestroyBonusDamageRatio = 0.05f;
	BreakableParts.Add(BodyPart);

	// 보스 주변 랜덤 원기둥 공격
	FTGBossPatternEntry RandomGroundCylinderEntry;
	RandomGroundCylinderEntry.PatternClass = UTGRandomGroundCylinderPattern::StaticClass();
	RandomGroundCylinderEntry.WarningRadius = 200.f;
	RandomGroundCylinderEntry.AttackDamage = 12.f;
	RandomGroundCylinderEntry.AttackEffectScale = 2.f;
	RandomGroundCylinderEntry.PatternCooldown = 8.f;
	RandomGroundCylinderEntry.MaxPlayerDistance = 1000.f;

	PatternEntries.Add(RandomGroundCylinderEntry);

	// 플레이어 구형 공격
	FTGBossPatternEntry SphereEntry;
	SphereEntry.PatternClass = UTGSphereAreaPattern::StaticClass();
	SphereEntry.WarningRadius = 150.f;
	SphereEntry.AttackDamage = 15.f;
	SphereEntry.AttackEffectScale = 2.f;

	PatternEntries.Add(SphereEntry);
}

void UTGSentinelBossPhase2::EnterPhase()
{
	Super::EnterPhase();
	UE_LOG(LogTemp, Warning, TEXT("Sentinel Boss Phase 2 Enter"));
}

void UTGSentinelBossPhase2::ExitPhase()
{
	UE_LOG(LogTemp, Warning, TEXT("Sentinel Boss Phase 2 Exit"));
	Super::ExitPhase();
}
