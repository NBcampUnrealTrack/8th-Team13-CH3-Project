// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TGWeaponBase.generated.h"

#define AMMO_UNDEFINED -1	//한번도 쏘지않은 상태

UENUM(BlueprintType)
enum class ETGWeaponTriggerType : uint8
{
	/*단발*/ SINGLE_SHOT UMETA(DisplayName = "SingleShot"),
	/*샷건*/ SHOTGUN UMETA(DisplayName = "Burst"),
	/*연사*/ REPEATER UMETA(DisplayName = "Repeater"),
	TRIGGER_COUNT UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FTGWeaponAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")	// 스켈레탈 메시 (스태틱메시보다 우선됨)
		USkeletalMesh* SkeletalMesh = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")	// 스태틱 메시 (스켈레탈 메시가 있으면 무시됨)
		UStaticMesh* StaticMesh = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")	// 총구 위치 (메시 기준)
		FVector MuzzlePos = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Offset")	// 위치 오프셋 (오차수정)
		FVector LocationOffset = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Offset")	// 회전 오프셋
		FRotator RotationOffset = FRotator::ZeroRotator;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualEffects")	// 발포 이펙트
		TObjectPtr<class UParticleSystem> FireParticle = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualEffects")	// 발포 이펙트 스케일
		float FireParticleScale = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualEffects")	// 착탄 이펙트
		TObjectPtr<class UParticleSystem> HitParticle = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualEffects")	// 착탄 이펙트 스케일
		float HitParticleScale = 1.f;
};

USTRUCT(BlueprintType)
struct FTGStatusWeaponBase : public FTableRowBase
{
	GENERATED_BODY()
	friend class UTGWeaponBase;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTGWeaponAsset Asset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Power = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxAmmo = 0;
};

UCLASS()
class TOWERGAME_API UTGWeaponBase : public UObject
{
	GENERATED_BODY()
protected:
	int32 CurAmmo = AMMO_UNDEFINED;
	bool CanFire = true;
protected:	// 라인트레이싱 정보
	FHitResult TraceHit;
	FCollisionQueryParams QueryParams;
	TArray<AActor*> IgnoredActors;

	FTimerHandle TimerFireDelay;
public:
	virtual const FTGWeaponAsset* GetAsset() { return nullptr; }
	virtual void Shoot(AActor* Instigator, class UMeshComponent* WeaponComponent, FVector MuzzlePos, FVector Direction, float Distance);
	void SpawnAttachedEffects(class UParticleSystem* particle, USceneComponent* AttachToComponent, FVector Location, float ParticleScale, bool bSpawnDecal = false, float DecalScale = 1.0f);
	void HandleFireDelay();
};
