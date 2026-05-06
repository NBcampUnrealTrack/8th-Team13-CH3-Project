// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TGCoreBase.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCoreDestroyed);

UCLASS()
class TOWERGAME_API ATGCoreBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATGCoreBase();

	virtual float TakeDamage(
		float DamageAmount,
		const FDamageEvent& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser
	) override;

	//UPROPERTY(BlueprintAssignable, Category = "Core")
	//FOnCoreDestroyed OnCoreDestroyed; //델리게이트 위해서 임의로 수정해뒀습니다.

	UFUNCTION(BlueprintCallable, Category="TowerGame|Status")
	float GetCurrentHP();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TowerGame|Status")
	float MaxHP = 100.f;
	float CurrentHP;

};
