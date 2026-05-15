// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/TGBossPhaseBase.h"

#include "Enemies/TGBossBase.h"

UTGBossPhaseBase::UTGBossPhaseBase() :
	OwnerBoss(nullptr),
	PatternInterval(3.f)
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
