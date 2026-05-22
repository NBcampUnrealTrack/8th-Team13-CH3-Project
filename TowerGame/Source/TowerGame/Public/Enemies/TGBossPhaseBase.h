// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TGBossPhaseBase.generated.h"

class UTGPatternBase;
class ATGBossBase;

USTRUCT(BlueprintType)
struct FTGBossPatternEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Pattern")
	FName LinkedPartTag = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Pattern")
	TSubclassOf<UTGPatternBase> PatternClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Pattern")
	float WarningRadius = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Pattern")
	float AttackDamage = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Pattern")
	float AttackEffectScale = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Pattern", meta = (ClampMin = "0.0"))
	float PatternCooldown = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Pattern", meta = (ClampMin = "0.0"))
	float MinPlayerDistance = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Pattern", meta = (ClampMin = "0.0"))
	float MaxPlayerDistance = 0.f;
};

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
	void CreatePatterns();
	bool CanUsePatternEntry(int32 PatternIndex) const;
	bool IsPlayerDistanceMatched(const FTGBossPatternEntry& Entry) const;

	// 이 Phase를 소유하고 실행하는 Boss.
	UPROPERTY()
	TObjectPtr<ATGBossBase> OwnerBoss;

	// Phase의 Pattern
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Pattern")
	TArray<FTGBossPatternEntry> PatternEntries;

	// PatternEntries를 기반으로 생성된 실제 런타임 Pattern 객체들
	UPROPERTY()
	TArray<TObjectPtr<UTGPatternBase>> Patterns;

	// Patterns와 같은 인덱스로 관리되는 Pattern별 마지막 실행 시간
	UPROPERTY()
	TArray<float> LastPatternStartTimes;

	// 현재 경고 표시 후 공격 대기 중인 Pattern
	UPROPERTY()
	TObjectPtr<UTGPatternBase> CurrentPattern;

	// 경고 표시 유지 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Pattern")
	float WarningDrawTime;

	// Phase 진입 후 ExecutePattern을 반복 호출하는 간격.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Pattern")
	float PatternInterval;

	// Phase 종료 시 현재 Phase의 패턴 호출만 정리하기 위한 타이머 핸들.
	FTimerHandle PatternTimerHandle;

	// 파괴 가능한 파츠/부위
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Part")
	TArray<FTGBossBreakablePartData> BreakableParts;

};
