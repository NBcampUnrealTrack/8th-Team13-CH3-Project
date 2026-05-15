// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/TGBossPhaseBase.h"

void UTGBossPhaseBase::Initialize(ATGBossBase* InOwnerBoss)
{
	OwnerBoss = InOwnerBoss;
}

void UTGBossPhaseBase::EnterPhase()
{
	UE_LOG(LogTemp, Log, TEXT("%s EnterPhase"), *GetName());
}

void UTGBossPhaseBase::ExitPhase()
{
	UE_LOG(LogTemp, Log, TEXT("%s ExitPhase"), *GetName());
}

void UTGBossPhaseBase::ExecutePattern()
{
	UE_LOG(LogTemp, Log, TEXT("%s ExecutePattern"), *GetName());
}
