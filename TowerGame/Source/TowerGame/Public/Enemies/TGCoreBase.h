// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TGCoreBase.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCoreDestroyed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCoreHPChanged, float, CurrentHP, float, MaxHp);

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

	// 체력 변동 전달
	UPROPERTY(BlueprintAssignable, Category = "Core")
	FOnCoreHPChanged OnCoreHPChanged;

	UFUNCTION(BlueprintCallable, Category="TowerGame|Status")
	float GetCurrentHP();

	UFUNCTION(BlueprintCallable, Category = "TowerGame|Status")
	float GetMaxHP();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	TObjectPtr<UParticleSystem> DestroyEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	float DestroyEffectScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TowerGame|Status")
	float MaxHP;
	float CurrentHP;

};
