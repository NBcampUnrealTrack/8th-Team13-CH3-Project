// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/TGWeaponBase.h"
#include "TGWeaponShotgun.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FTGStatusWeaponShotgun : public FTGStatusWeaponBase
{
	GENERATED_BODY()
	friend class UTGWeaponShotgun;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	// 사격주기
		float ShotInterval = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	// 발당 산탄 수
		int32 Pellet = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	// 탄 퍼짐 각도 (낮을 수록 정확도 상승)
		float BulletSpread = 0.f;
};

UCLASS()
class TOWERGAME_API UTGWeaponShotgun : public UTGWeaponBase
{
	GENERATED_BODY()
private:
	UPROPERTY()
	FTGStatusWeaponShotgun status;
public:
	virtual void Shoot(class ATGPlayer* Instigator, class UMeshComponent* WeaponComponent, FVector MuzzlePos, FVector Direction, float Distance, bool TriggerLock) override;
	virtual const FTGWeaponAsset* GetAsset() override { return &status.Asset; };
	void SetStatus(const FTGStatusWeaponShotgun value) { status = value; }
};
