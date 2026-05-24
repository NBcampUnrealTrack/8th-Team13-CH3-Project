// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TGPartBreakComponent.generated.h"

class ATGBossBase;
class UMaterialInstanceDynamic;
class UParticleSystem;
class USoundBase;
class UStaticMeshComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnDetachedPartRemoved, FName);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TOWERGAME_API UTGPartBreakComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTGPartBreakComponent();

	void Initialize(ATGBossBase* InOwnerBoss);

	void BreakPartsByTag(FName PartTag);
	void RemoveDetachedPartComponent(UStaticMeshComponent* StaticMesh, FName PartTag);

	void RebuildPartDamageMaterialCache(const TSet<FName>& ActiveBreakablePartTags);
	void UpdateBreakablePartDamageVisual(FName PartTag, float CurrentPartHP, float MaxPartHP);

	FOnDetachedPartRemoved OnDetachedPartRemoved;

protected:
	UPROPERTY()
	TObjectPtr<ATGBossBase> OwnerBoss;

	// 부위 파괴 Sound
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss|Part|Sound")
	TObjectPtr<USoundBase> PartBreakSound;

	// 부위 파괴 Effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss|Part|Effect")
	TObjectPtr<UParticleSystem> PartBreakEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Part|Effect")
	float EffectScale;

private:
	UPROPERTY()
	TSet<FName> ActiveBreakablePartTags;

	float PartsLifeSpan;
	float ExplodeRadius;
	float ExplodeForce;

	// 부위 외형 손상용 캐시
	TMap<FName, TArray<UMaterialInstanceDynamic*>> PartDamageMaterialMap;
};
