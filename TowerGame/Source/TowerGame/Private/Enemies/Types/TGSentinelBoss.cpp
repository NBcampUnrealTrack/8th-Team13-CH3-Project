// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Types/TGSentinelBoss.h"

#include "Enemies/Phase/TGSentinelBossPhase1.h"
#include "Enemies/Phase/TGSentinelBossPhase2.h"
#include "Enemies/Phase/TGSentinelBossPhase3.h"

ATGSentinelBoss::ATGSentinelBoss()
{
	MaxHP = 1000;

	PhaseHPRatio.Add(0.6f);
	PhaseHPRatio.Add(0.3f);

	PhaseClasses.Add(UTGSentinelBossPhase1::StaticClass());
	PhaseClasses.Add(UTGSentinelBossPhase2::StaticClass());
	PhaseClasses.Add(UTGSentinelBossPhase3::StaticClass());
}
