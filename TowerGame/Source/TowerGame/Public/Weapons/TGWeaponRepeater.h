// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/TGWeaponBase.h"
#include "TGWeaponRepeater.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FTGStatusWeaponRepeater : public FTGStatusWeaponBase
{
	GENERATED_BODY()
	friend class UTGWeaponRepeater;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	// 사격주기
		float ShotInterval;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	// 탄 퍼짐 (낮을 수록 정확도 상승)
		float BulletSpead;
};

UCLASS()
class TOWERGAME_API UTGWeaponRepeater : public UTGWeaponBase
{
	GENERATED_BODY()
private:
	FTGStatusWeaponRepeater status;
public:
	virtual void Shoot(AActor* Instigator, class UMeshComponent* WeaponComponent, FVector MuzzlePos, FVector Direction, float Distance) override;
	virtual const FTGWeaponAsset* GetAsset() override { return &status.Asset; };
	void SetStatus(const FTGStatusWeaponRepeater value) { status = value; }
};
