// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Phase/TGSentinelBossPhase1.h"
#include "Enemies/Pattern/TGGroundCylinderPattern.h"

UTGSentinelBossPhase1::UTGSentinelBossPhase1()
{
	// Phase 파괴 가능 Part 설정
	FTGBossBreakablePartData LeftArmPart;
	LeftArmPart.PartTag = TEXT("L_Arm");
	LeftArmPart.HPRatio = 0.1f;
	LeftArmPart.DestroyBonusDamageRatio = 0.05f;
	BreakableParts.Add(LeftArmPart);

	FTGBossBreakablePartData RightArmPart;
	RightArmPart.PartTag = TEXT("R_Arm");
	RightArmPart.HPRatio = 0.1f;
	RightArmPart.DestroyBonusDamageRatio = 0.05f;
	BreakableParts.Add(RightArmPart);

	// 플레이어 원통 공격
	FTGBossPatternEntry GroundCylinderEntry;
	GroundCylinderEntry.LinkedPartTag = TEXT("L_Arm");
	GroundCylinderEntry.PatternClass = UTGGroundCylinderPattern::StaticClass();
	GroundCylinderEntry.WarningRadius = 300.f;
	GroundCylinderEntry.AttackDamage = 10.f;
	GroundCylinderEntry.AttackEffectScale = 5.f;

	PatternEntries.Add(GroundCylinderEntry);
}

void UTGSentinelBossPhase1::EnterPhase()
{
	Super::EnterPhase();
	UE_LOG(LogTemp, Warning, TEXT("Sentinel Boss Phase 1 Enter"));
}

void UTGSentinelBossPhase1::ExitPhase()
{
	UE_LOG(LogTemp, Warning, TEXT("Sentinel Boss Phase 1 Exit"));
	Super::ExitPhase();
}
