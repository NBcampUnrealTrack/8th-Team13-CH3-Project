// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/TGCoreBase.h"

#include "Core/GameFlow/TGGameMode.h"
#include "Enemies/TGNavigationManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATGCoreBase::ATGCoreBase() : MaxHP(100)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(SceneRoot);

	CurrentHP = MaxHP;
}

float ATGCoreBase::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	const float AppliedDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 체력 변동 전달
	CurrentHP = FMath::Clamp(CurrentHP - AppliedDamage, 0.0f, MaxHP);
	OnCoreHPChanged.Broadcast(CurrentHP, MaxHP);

	if (CurrentHP <= 0)
	{
		if (DestroyEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				this,
				DestroyEffect,
				GetActorLocation(),
				FRotator::ZeroRotator,
				FVector::OneVector * DestroyEffectScale
			);
		}

		if (ATGGameMode* GM = Cast<ATGGameMode>(GetWorld()->GetAuthGameMode()))
		{
			//	TODO : 게임 오버 처리
			GM->HandleGameOver();
		}
		//UE_LOG(LogTemp, Warning, TEXT("CoreBase 파괴"));
		//OnCoreDestroyed.Broadcast(); //델리게이트 이벤트 발생
		//UE_LOG(LogTemp, Warning, TEXT("델리게이트 발생 완료"));
		Destroy();
	}

	return AppliedDamage;
}

float ATGCoreBase::GetCurrentHP() { return CurrentHP; }

float ATGCoreBase::GetMaxHP() { return MaxHP; }

// Called when the game starts or when spawned
void ATGCoreBase::BeginPlay()
{
	Super::BeginPlay();

	if (ATGNavigationManager* NavigationManager = ATGNavigationManager::Get(this)){
		NavigationManager->AddCoreActor(this);
	}
}

void ATGCoreBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ATGNavigationManager* NavigationManager = ATGNavigationManager::Get(this)){
		NavigationManager->RemoveCoreActor(this);
	}

	Super::EndPlay(EndPlayReason);
}

