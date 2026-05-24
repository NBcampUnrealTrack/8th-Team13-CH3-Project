// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/TGPartBreakComponent.h"

#include "Components/StaticMeshComponent.h"
#include "Enemies/TGBossBase.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"

UTGPartBreakComponent::UTGPartBreakComponent() :
	OwnerBoss(nullptr),
	PartBreakSound(nullptr),
	PartBreakEffect(nullptr),
	EffectScale(1.f),
	PartsLifeSpan(3.f),
	ExplodeRadius(100.f),
	ExplodeForce(50.f)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTGPartBreakComponent::Initialize(ATGBossBase* InOwnerBoss)
{
	OwnerBoss = InOwnerBoss;
}

void UTGPartBreakComponent::BreakPartsByTag(FName PartTag)
{
	if (!OwnerBoss || PartTag == NAME_None) return;

	// 보스의 하위 StaticMeshComponent 전체 수집
	TArray<UStaticMeshComponent*> StaticMeshComponents;
	OwnerBoss->GetComponents<UStaticMeshComponent>(StaticMeshComponents);

	bool bPlayedPartBreak = false;

	for (UStaticMeshComponent* StaticMesh: StaticMeshComponents){
		// 태그가 일치하는 컴포넌트만 필터링
		if (!StaticMesh || !StaticMesh->ComponentHasTag(PartTag)) continue;

		// 부모로부터 분리 및 충돌/네비/물리 설정
		StaticMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		StaticMesh->SetCanEverAffectNavigation(false);
		StaticMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		StaticMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		StaticMesh->SetSimulatePhysics(true);

		// 폭발 적용(Force)
		StaticMesh->AddRadialForce(
			OwnerBoss->GetActorLocation(),
			ExplodeRadius,
			ExplodeForce,
			RIF_Linear,
			true
		);

		// 폭발 적용(Rotation)
		StaticMesh->AddTorqueInDegrees(FVector(
			FMath::FRandRange(-180.f,180.f),
			FMath::FRandRange(-180.f,180.f),
			FMath::FRandRange(-180.f,180.f)
		));

		// 폭발 사운드 / 이펙트 1회
		if (!bPlayedPartBreak){
			if (PartBreakSound){
				UGameplayStatics::PlaySoundAtLocation(
					this,
					PartBreakSound,
					StaticMesh->GetComponentLocation(),
					1.f
				);
			}

			if (PartBreakEffect){
				UGameplayStatics::SpawnEmitterAtLocation(
					this,
					PartBreakEffect,
					StaticMesh->GetComponentLocation(),
					StaticMesh->GetComponentRotation(),
					FVector(EffectScale)
				);
			}

			bPlayedPartBreak = true;
		}

		UWorld* World = GetWorld();
		if (!World) continue;

		// 파괴된 파츠가 Level에서 사라질 때 호출됨
		FTimerHandle DestroyPartTimerHandle;
		FTimerDelegate DestroyPartDelegate;
		DestroyPartDelegate.BindUObject(
			this,
			&UTGPartBreakComponent::RemoveDetachedPartComponent,
			StaticMesh,
			PartTag
		);

		World->GetTimerManager().SetTimer(
			DestroyPartTimerHandle,
			DestroyPartDelegate,
			PartsLifeSpan,
			false
		);
	}
}

void UTGPartBreakComponent::RemoveDetachedPartComponent(UStaticMeshComponent* StaticMesh, FName PartTag)
{
	if (StaticMesh && !StaticMesh->IsBeingDestroyed()){
		StaticMesh->DestroyComponent();
	}

	// Tag 및 캐시 제거
	ActiveBreakablePartTags.Remove(PartTag);
	PartDamageMaterialMap.Remove(PartTag);
	OnDetachedPartRemoved.Broadcast(PartTag);
}

void UTGPartBreakComponent::RebuildPartDamageMaterialCache(const TSet<FName>& InActiveBreakablePartTags)
{
	ActiveBreakablePartTags = InActiveBreakablePartTags;
	PartDamageMaterialMap.Empty();

	if (!OwnerBoss) return;

	TArray<UStaticMeshComponent*> StaticMeshComponents;
	OwnerBoss->GetComponents<UStaticMeshComponent>(StaticMeshComponents);

	for (UStaticMeshComponent* StaticMesh : StaticMeshComponents){
		if (!StaticMesh) continue;

		for (const FName& PartTag : ActiveBreakablePartTags){
			if (!StaticMesh->ComponentHasTag(PartTag)) continue;

			// 다이나믹 머티리얼 인스턴스 cast
			UMaterialInstanceDynamic* DynamicMaterial =
				Cast<UMaterialInstanceDynamic>(StaticMesh->GetMaterial(0));

			if (!DynamicMaterial){
				DynamicMaterial = StaticMesh->CreateAndSetMaterialInstanceDynamic(0);
			}

			// 다이나믹 머티리얼 캐시에 저장
			if (DynamicMaterial){
				PartDamageMaterialMap.FindOrAdd(PartTag).Add(DynamicMaterial);
			}
		}
	}
}

void UTGPartBreakComponent::UpdateBreakablePartDamageVisual(
	FName PartTag,
	float CurrentPartHP,
	float MaxPartHP)
{
	if (MaxPartHP <= 0.f) return;

	// 체력 비율 적용
	const float HPRatio = CurrentPartHP / MaxPartHP;
	const float DamageAmount = 1.f - HPRatio;

	TArray<UMaterialInstanceDynamic*>* PartMaterials = PartDamageMaterialMap.Find(PartTag);
	if (!PartMaterials) return;

	for (UMaterialInstanceDynamic* Material : *PartMaterials){
		if (!Material) continue;

		Material->SetScalarParameterValue(TEXT("PartDamageAmount"), static_cast<float>(DamageAmount * 2));
	}
}
