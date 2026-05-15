// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Phase/TGSentinelBossPhase1.h"

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

void UTGSentinelBossPhase1::ExecutePattern()
{
	UE_LOG(LogTemp, Warning, TEXT("Sentinel Boss Phase 1 Pattern"));
}
