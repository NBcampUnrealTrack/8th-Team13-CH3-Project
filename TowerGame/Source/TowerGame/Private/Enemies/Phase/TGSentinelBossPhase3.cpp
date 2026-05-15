// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Phase/TGSentinelBossPhase3.h"

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

void UTGSentinelBossPhase3::ExecutePattern()
{
	UE_LOG(LogTemp, Warning, TEXT("Sentinel Boss Phase 3 Pattern"));
}
