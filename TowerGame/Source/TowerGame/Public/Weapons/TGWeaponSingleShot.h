// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/TGWeaponBase.h"
#include "TGWeaponSingleShot.generated.h"

/**
 *
 */
USTRUCT(BlueprintType)
struct FTGStatusWeaponSingleShot : public FTGStatusWeaponBase
{
	GENERATED_BODY()
	friend class UTGWeaponSingleShot;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	// 사격쿨타임
		float ShotCoolTime;
};

UCLASS()
class TOWERGAME_API UTGWeaponSingleShot : public UTGWeaponBase
{
	GENERATED_BODY()
private:
	FTGStatusWeaponSingleShot status;
public:
	virtual void Shoot(AActor* Instigator, class UMeshComponent* WeaponComponent, FVector TargetPos) override;
	virtual const FTGWeaponAsset* GetAsset() override { return &status.Asset; };
	void SetStatus(const FTGStatusWeaponSingleShot value) { status = value; }
};
