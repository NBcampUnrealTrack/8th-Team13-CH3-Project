// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TGWeaponBase.generated.h"

UENUM()
enum EWeaponTriggerType
{
	SINGLE_SHOT,	// 단발
	BURST,			// 점사
	REPEATER		// 연발
};

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
	FName Name;
	AActor* Owner;
	EWeaponTriggerType TriggerType;
	int32 Power;
	//int Ammo;
	//int Magazine;
public:
	void SetOwner(AActor* owner) { Owner = owner; }
	void Hit(AActor* target);
};
