// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TGTargetable.generated.h"

// 타워가 조준/공격/디버프할 수 있는 대상을 나타내는 인터페이스.
// 적 구현이 무엇이든(경량 Actor 등) 타워는 이 인터페이스로만 상호작용한다 → 타겟팅 일원화.
UINTERFACE(MinimalAPI)
class UTGTargetable : public UInterface
{
	GENERATED_BODY()
};

class ITGTargetable
{
	GENERATED_BODY()

public:
	// 타워의 타겟이 될 수 있는 상태인지 (살아있고 유효)
	virtual bool IsTargetable() const = 0;

	// 이동 슬로우 배율 설정 (1.0 = 정상, 0.5 = 50% 감속). 디버프 타워가 호출.
	virtual void SetMoveSlowMultiplier(float Multiplier) = 0;
};
