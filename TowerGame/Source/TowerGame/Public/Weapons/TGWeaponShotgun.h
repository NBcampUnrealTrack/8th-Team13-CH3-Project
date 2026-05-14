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
		float ShotInterval;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	// 발당 산탄 수
		int32 Pellet;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	// 탄 퍼짐 각도 (낮을 수록 정확도 상승)
		float BulletSpread;
};

UCLASS()
class TOWERGAME_API UTGWeaponShotgun : public UTGWeaponBase
{
	GENERATED_BODY()
private:
	FTGStatusWeaponShotgun status;
public:
	virtual void Shoot(AActor* Instigator, class UMeshComponent* WeaponComponent, FVector MuzzlePos, FVector Direction, float Distance) override;
	virtual const FTGWeaponAsset* GetAsset() override { return &status.Asset; };
	void SetStatus(const FTGStatusWeaponShotgun value) { status = value; }
};
