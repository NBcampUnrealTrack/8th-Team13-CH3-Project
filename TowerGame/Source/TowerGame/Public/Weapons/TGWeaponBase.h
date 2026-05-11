// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TGWeaponBase.generated.h"

UENUM(BlueprintType)
enum class EWeaponTriggerType : uint8
{
	/*단발*/ SINGLE_SHOT UMETA(DisplayName = "SingleShot"),
	/*점사*/ BURST UMETA(DisplayName = "Burst"),
	/*연사*/ REPEATER UMETA(DisplayName = "Repeater"),
	TRIGGER_COUNT UMETA(Hidden)
};

//USTRUCT(BlueprintType)
//struct FTGStatusWeaponBase
//{
//	GENERATED_BODY()
//protected:
//	FName Name;
//	int32 Power;
//	//int32 Ammo;
//	//int32 Magazine;
//};

UCLASS()
class TOWERGAME_API ATGWeaponBase : public AActor
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	ATGWeaponBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
protected:
	AActor* Owner;
	EWeaponTriggerType TriggerType;
private:
	//FTGStatusWeaponBase info;
public:
	void SetOwner(AActor* owner) { Owner = owner; }
	void Shoot(AActor* target);
};
