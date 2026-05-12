// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TGWeaponBase.generated.h"

UENUM(BlueprintType)
enum class ETGWeaponTriggerType : uint8
{
	/*단발*/ SINGLE_SHOT UMETA(DisplayName = "SingleShot"),
	/*점사*/ BURST UMETA(DisplayName = "Burst"),
	/*연사*/ REPEATER UMETA(DisplayName = "Repeater"),
	TRIGGER_COUNT UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FTGWeaponAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	// 스켈레탈 메시 (스태틱메시보다 우선됨)
		USkeletalMesh* SkeletalMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	// 스태틱 메시 (스켈레탈 메시가 있으면 무시됨)
		UStaticMesh* StaticMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	// 총구 위치 (메시 기준)
		FVector MuzzlePos;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	// 위치 오프셋 (오차수정)
		FVector LocationOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	// 회전 오프셋
		FRotator RotationOffset;
};

USTRUCT(BlueprintType)
struct FTGStatusWeaponBase : public FTableRowBase
{
	GENERATED_BODY()
	friend class UTGWeaponBase;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTGWeaponAsset Asset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Power;
	//int32 Ammo;
	//int32 Magazine;
};

UCLASS()
class TOWERGAME_API UTGWeaponBase : public UObject
{
	GENERATED_BODY()
protected:
	ETGWeaponTriggerType TriggerType;
public:
	virtual const FTGWeaponAsset* GetAsset() { return nullptr; }
	virtual void Shoot(AActor* Instigator, class UMeshComponent* WeaponComponent, FVector TargetPos) {}
};
