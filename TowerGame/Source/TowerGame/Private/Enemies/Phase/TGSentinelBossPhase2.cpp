// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Phase/TGSentinelBossPhase2.h"

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

void UTGSentinelBossPhase2::ExecutePattern()
{
	UE_LOG(LogTemp, Warning, TEXT("Sentinel Boss Phase 2 Pattern"));
}
