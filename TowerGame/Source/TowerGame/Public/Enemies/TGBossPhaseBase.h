// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TGBossPhaseBase.generated.h"

class ATGBossBase;

USTRUCT(BlueprintType)
struct FTGBossBreakablePartData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Part")
	FName PartTag = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Part")
	float HPRatio = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Part")
	float CurrentHP = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Part")
	float DestroyBonusDamageRatio = 0.f;
};

UCLASS()
class TOWERGAME_API UTGBossPhaseBase : public UObject
{
	GENERATED_BODY()

public:
	UTGBossPhaseBase();

	virtual void Initialize(ATGBossBase* InOwnerBoss);

	// Phase 관리
	virtual void EnterPhase();
	virtual void ExitPhase();

	// 세부 로직은 각 Phase별 구성
	virtual void ExecutePattern();

	FTGBossBreakablePartData* FindBreakablePart(FName PartTag);
	void RemoveBreakablePart(FName PartTag);

protected:
	// 이 Phase를 소유하고 실행하는 Boss.
	UPROPERTY()
	TObjectPtr<ATGBossBase> OwnerBoss;

	// Phase 진입 후 ExecutePattern을 반복 호출하는 간격.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Pattern")
	float PatternInterval;

	// Phase 종료 시 현재 Phase의 패턴 호출만 정리하기 위한 타이머 핸들.
	FTimerHandle PatternTimerHandle;

	// 파괴 가능한 파츠/부위
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Part")
	TArray<FTGBossBreakablePartData> BreakableParts;
};
