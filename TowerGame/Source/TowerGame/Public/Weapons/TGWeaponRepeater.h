// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tickable.h"
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
		float ShotInterval = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	// 탄 퍼짐 최대각도 (낮을 수록 정확도 상승)
		float MaxBulletSpread = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	// 탄 퍼짐이 최대치에 도달하기까지의 시간(초)
		float MaxSpreadTime = 0.f;
};

UCLASS()
class TOWERGAME_API UTGWeaponRepeater : public UTGWeaponBase, public FTickableGameObject
{
	GENERATED_BODY()
private:
	FTGStatusWeaponRepeater status;
	float CurSpreadTime = 0.f;
	bool IsFire = false;	// 이번 프레임에 발사시도를 했는가?
public:
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual bool IsTickableInEditor() const override;
	virtual bool IsTickableWhenPaused() const override;
	virtual TStatId GetStatId() const override;
	virtual UWorld* GetWorld() const override;

	virtual void Shoot(AActor* Instigator, class UMeshComponent* WeaponComponent, FVector MuzzlePos, FVector Direction, float Distance) override;
	virtual const FTGWeaponAsset* GetAsset() override { return &status.Asset; };
	void SetStatus(const FTGStatusWeaponRepeater value) { status = value; }
};
