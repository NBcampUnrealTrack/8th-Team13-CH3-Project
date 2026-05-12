// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/TGWeaponSingleShot.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/MeshComponent.h"
#include "Enemies/TGEnemyBase.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"

void UTGWeaponSingleShot::Shoot(AActor* Instigator, UMeshComponent* WeaponComponent, FVector TargetPos)
{
	FHitResult TraceHit;
	FCollisionQueryParams QueryParams;
	TArray<AActor*> IgnoredActors = { Instigator };
	QueryParams.AddIgnoredActors(IgnoredActors);

	FVector StartPos = WeaponComponent->GetComponentTransform().TransformPosition(status.Asset.MuzzlePos);

	UKismetSystemLibrary::LineTraceSingle(
		GetWorld(), //어느 월드의 소속인가? (this)를 넣어줘도 됨
		StartPos,
		TargetPos,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),	// 사용할 트레이스채널
		QueryParams.bTraceComplex,	// 복합콜리전 사용
		IgnoredActors,	// 해당 액터는 이 트레이스를 무시
		EDrawDebugTrace::ForDuration,	//디버그(그리기 타입 적용),
		TraceHit,
		true,	// 자기자신을 Ignore
		FLinearColor::Blue,	//디버그 색깔
		FLinearColor::Yellow,	//트레이스 히트 시 색깔
		5.0f
	);
	if (TraceHit.bBlockingHit && TraceHit.GetActor())
	{
		ATGEnemyBase* target = Cast<ATGEnemyBase>(TraceHit.GetActor());
		if (target)
		{
			UGameplayStatics::ApplyDamage(target,status.Power,Instigator->GetInstigatorController(), Instigator, nullptr);
		}
	}
}
