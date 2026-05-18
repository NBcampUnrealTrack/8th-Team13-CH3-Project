// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/TGWeaponRepeater.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Enemies/TGBossBase.h"
#include "Enemies/TGEnemyBase.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"

void UTGWeaponRepeater::Tick(float DeltaTime)
{
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, FString::Printf(TEXT("SpreadTime: %f/%f"), CurSpreadTime, status.MaxSpreadTime));
	if (IsFire)
		IsFire = false;
	else
	{
		CurSpreadTime -= status.MaxSpreadTime * DeltaTime * 2.0f;
		if (CurSpreadTime <= 0.0f)
			CurSpreadTime = 0.0f;
	}
}

bool UTGWeaponRepeater::IsTickable() const
{
	return CurSpreadTime > 0.0f;
}

bool UTGWeaponRepeater::IsTickableInEditor() const
{
	return false;
}

bool UTGWeaponRepeater::IsTickableWhenPaused() const
{
	return false;
}

TStatId UTGWeaponRepeater::GetStatId() const
{
	return TStatId();
}

UWorld* UTGWeaponRepeater::GetWorld() const
{
	return GetOuter()->GetWorld();
}

void UTGWeaponRepeater::Shoot(AActor* Instigator, class UMeshComponent* WeaponComponent, FVector MuzzlePos, FVector Direction, float Distance)
{
	IsFire = true;
	CurSpreadTime += GetWorld()->GetDeltaSeconds();
	if (CurSpreadTime > status.MaxSpreadTime)
		CurSpreadTime = status.MaxSpreadTime;

	if (!CanFire)
		return;

	CanFire = false;
	GetWorld()->GetTimerManager().SetTimer(TimerFireDelay, this, &UTGWeaponBase::HandleFireDelay, status.ShotInterval, false);
	Super::Shoot(Instigator, WeaponComponent, MuzzlePos, Direction, Distance);

	// 발포 이펙트
	SpawnAttachedEffects(status.Asset.FireParticle, WeaponComponent, MuzzlePos, status.Asset.FireParticleScale);
	
	FVector SpeadDir = FMath::VRandCone(Direction, FMath::DegreesToRadians(FMath::Lerp(0.0f,status.MaxBulletSpread,(CurSpreadTime/status.MaxSpreadTime))));	//탄퍼짐 각도
	UKismetSystemLibrary::LineTraceSingle(
		GetWorld(), //어느 월드의 소속인가? (this)를 넣어줘도 됨
		MuzzlePos,
		MuzzlePos + SpeadDir * Distance,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),	// 사용할 트레이스채널
		QueryParams.bTraceComplex,	// 복합콜리전 사용
		IgnoredActors,	// 해당 액터는 이 트레이스를 무시
		EDrawDebugTrace::None,	//디버그(그리기 타입 적용),
		TraceHit,
		true,	// 자기자신을 Ignore
		FLinearColor::Blue,	//디버그 색깔
		FLinearColor::Yellow,	//트레이스 히트 시 색깔
		5.0f
	);
	if (TraceHit.bBlockingHit)
	{
		// 착탄 이펙트
		SpawnAttachedEffects(status.Asset.HitParticle, TraceHit.GetActor()->GetRootComponent(), TraceHit.Location, status.Asset.HitParticleScale, true);

		AActor* HitActor = TraceHit.GetActor();
		if (ATGEnemyBase* Enemy = Cast<ATGEnemyBase>(HitActor))
		{
			UGameplayStatics::ApplyDamage(Enemy, status.Power, Instigator->GetInstigatorController(), Instigator, nullptr);
		}
		else if (ATGBossBase* Boss = Cast<ATGBossBase>(HitActor))
		{
			UGameplayStatics::ApplyDamage(Boss, status.Power, Instigator->GetInstigatorController(), Instigator, nullptr);
		}
	}
}
