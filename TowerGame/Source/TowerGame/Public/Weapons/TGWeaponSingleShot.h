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
		float ShotCoolTime = 0.f;
};

UCLASS()
class TOWERGAME_API UTGWeaponSingleShot : public UTGWeaponBase, public FTickableGameObject
{
	GENERATED_BODY()
private:
	FTGStatusWeaponSingleShot status;
	bool IsFire = false;		// 이번 프레임에 발사시도를 했는가?
	bool IsPrevFire = false;		// 이전 프레임에 발사시도를 했는가?
public:
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual bool IsTickableInEditor() const override;
	virtual bool IsTickableWhenPaused() const override;
	virtual TStatId GetStatId() const override;
	virtual UWorld* GetWorld() const override;

	virtual void Shoot(class ATGPlayer* Instigator, class UMeshComponent* WeaponComponent, FVector MuzzlePos, FVector Direction, float Distance, bool TriggerLock) override;
	virtual const FTGWeaponAsset* GetAsset() override { return &status.Asset; };
	void SetStatus(const FTGStatusWeaponSingleShot value) { status = value; }
};
