// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TGPerkTypes.generated.h"

// 특성(Perk) 효과 종류 — 적용 로직은 UTGPerkComponent::ApplyPerk 에서 분기 처리한다.
UENUM(BlueprintType)
enum class ETGPerkEffect : uint8
{
	/*최대 체력 증가 (Magnitude = 증가량, 현재 체력도 함께 회복)*/	MaxHpUp         UMETA(DisplayName = "최대 체력 증가"),
	/*체력 회복 (Magnitude = 회복량)*/							Heal            UMETA(DisplayName = "체력 회복"),
	/*이동 속도 증가 (Magnitude = 증가 비율, 0.1 = +10%)*/		MoveSpeedUp     UMETA(DisplayName = "이동 속도 증가"),
	/*무기 데미지 증가 (Magnitude = 배수 증가분, 0.15 = +15%)*/	WeaponDamageUp  UMETA(DisplayName = "무기 데미지 증가"),
	/*회피 횟수 증가 (Magnitude = 증가 횟수)*/					EvadeCountUp    UMETA(DisplayName = "회피 횟수 증가"),
	/*타워 건설 기회 (Magnitude = 지급 토큰 수)*/				GrantBuildToken UMETA(DisplayName = "타워 건설 기회")
};

// 특성 1종을 정의하는 DataTable 행. (DataTable Row Struct = FTGPerkData)
USTRUCT(BlueprintType)
struct FTGPerkData : public FTableRowBase
{
	GENERATED_BODY()

	// 특성 식별용 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perk")
	FName PerkId = NAME_None;

	// 선택 화면에 표시할 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perk")
	FText DisplayName;

	// 선택 화면에 표시할 설명
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perk", meta = (MultiLine = true))
	FText Description;

	// 아이콘
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perk")
	TObjectPtr<UTexture2D> Icon = nullptr;

	// 효과 종류
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perk")
	ETGPerkEffect Effect = ETGPerkEffect::MaxHpUp;

	// 효과 크기 (효과 종류별 해석이 다름 — ETGPerkEffect 주석 참고)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perk")
	float Magnitude = 0.f;

	// 추첨 가중치 (클수록 자주 등장, 0이면 등장하지 않음)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perk", meta = (ClampMin = "0"))
	int32 Weight = 100;
};
