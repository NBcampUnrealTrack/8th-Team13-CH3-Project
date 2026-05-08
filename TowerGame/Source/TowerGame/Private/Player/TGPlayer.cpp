// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TGPlayer.h"
#include "EnhancedInputComponent.h"
#include "Player/TGPlayerController.h"
#include "Camera/CameraComponent.h"
#include "InputActionValue.h"
#include "Core/Grid/TGGridBase.h"
#include "TGInteractiveActor.h"
#include "Enemies/TGEnemyBase.h"
#include "Engine/DamageEvents.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ATGPlayer::ATGPlayer() : MaxHP(100)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(RootComponent);
	Camera->bUsePawnControlRotation = true;

	EvadeCount = 2;
	EvadeCooldown = 3.0f;
	InteractDistance = 300.f;
	CurrentFocusedActor = nullptr;
}

// Called when the game starts or when spawned
void ATGPlayer::BeginPlay()
{
	Super::BeginPlay();
	HP = MaxHP;
	bMoving = false;
	bBuildMode = false;
	CurrentEvadeCount = EvadeCount;
	CurrentEvadeCooldown = 0.0f;

	if (GetWorld()->GetFirstPlayerController())
		EnableInput(GetWorld()->GetFirstPlayerController());
}

void ATGPlayer::Move(const FInputActionValue& value)
{
	if (!Controller) return;
	bMoving = true;
	FVector2D MoveInput = value.Get<FVector2D>();
	MoveDir = FVector2D::ZeroVector;
	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		MoveDir.Y = MoveInput.Y;
		AddMovementInput(GetActorForwardVector(), MoveInput.Y);
	}
	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		MoveDir.X = MoveInput.X;
		AddMovementInput(GetActorRightVector(), MoveInput.X);
	}
	if (!MoveDir.IsNearlyZero())
		MoveDir.Normalize();
}

void ATGPlayer::Look(const FInputActionValue& value)
{
	if (!Controller) return;
	FVector2D LookInput = value.Get<FVector2D>();
	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void ATGPlayer::JumpAction(const struct FInputActionValue& value)
{
	if (!Controller) return;
	if(!bWasJumping)
		Jump();
}

void ATGPlayer::Evade(const FInputActionValue& value)
{
	if (!Controller) return;

	FVector LaunchVel;	// 추진속도
	const float XYVelocity = 1500.0f;
	const float ZVelocity = 200.0f;

	if (CurrentEvadeCount <= 0)
		return;
	CurrentEvadeCount--;

	if (MoveDir.IsNearlyZero())
		LaunchVel = GetActorForwardVector() * XYVelocity + FVector(0.0f, 0.0f, ZVelocity);
	else
	{
		LaunchVel = GetActorForwardVector() * MoveDir.Y + GetActorRightVector() * MoveDir.X;
		LaunchVel.Normalize();
		LaunchVel = LaunchVel * XYVelocity + FVector(0.0f, 0.0f, ZVelocity);
	}

	LaunchCharacter(LaunchVel, true, true);
}

void ATGPlayer::Build(const FInputActionValue& InputValue)
{
	bBuildMode = !bBuildMode;
}

void ATGPlayer::Shot(const FInputActionValue& InputValue)
{
	FHitResult TraceHit;
	if (CameraLineTrace(TraceHit, ECC_Visibility))
	{
		ATGEnemyBase* target = Cast<ATGEnemyBase>(TraceHit.GetActor());
		if (target)
		{
			FDamageEvent DamEvent;
			target->TakeDamage(6.0f, DamEvent, GetController(), this);

		}
	}
}

void ATGPlayer::Interact(const FInputActionValue& InputValue)
{
	if (bBuildMode && CurrentFocusedActor)
		CurrentFocusedActor->OnInteract(this);
}

// Called every frame
void ATGPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RestoreEvadeCooldown(DeltaTime);	// 회피기동 쿨타임 회복

	FHitResult debugTemp;
	if (bBuildMode)
	{
		InteractiveTrace();
	}
	else
	{
		if (CurrentFocusedActor)
		{
			CurrentFocusedActor->OnUnfocused(this);

			CurrentFocusedActor = nullptr;
		}
		// 시각화 디버그용 (UI 추가 후 삭제)
		CameraLineTrace(debugTemp, ECC_Visibility, 5000.0f, true);
	}

	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Cyan, FString::Printf(TEXT("Current Evade Cooldown: %f / %f"), CurrentEvadeCooldown, EvadeCooldown));
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Cyan, FString::Printf(TEXT("Current Evade Count(LSHIFT): %d / %d"), CurrentEvadeCount, EvadeCount));
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Cyan, FString::Printf(TEXT("Aim Target: %s"), debugTemp.bBlockingHit ? *debugTemp.GetActor()->GetName() : TEXT("None")));
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, bBuildMode ? FColor::Emerald : FColor::Orange, FString::Printf(TEXT("Current Mode: %s"), bBuildMode ? TEXT("Build") : TEXT("Combat")));


	// 현재 이동조작중인가?
	if (bMoving)
		bMoving = false;
	else
		MoveDir = FVector2D::ZeroVector;

}

void ATGPlayer::RestoreEvadeCooldown(float DeltaTime)
{
	if (CurrentEvadeCount < EvadeCount)
	{
		CurrentEvadeCooldown += DeltaTime;
		if (CurrentEvadeCooldown >= EvadeCooldown)
		{
			CurrentEvadeCooldown = 0.0f;
			CurrentEvadeCount++;
		}
	}
	else
	{
		CurrentEvadeCooldown = 0.0f;
	}
}

// Called to bind functionality to input
void ATGPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (ATGPlayerController* PlayerController = Cast<ATGPlayerController>(GetController()))
		{
			if (PlayerController->Action_Move)
				EnhancedInputComponent->BindAction(PlayerController->Action_Move, ETriggerEvent::Triggered, this, &ATGPlayer::Move);
			if (PlayerController->Action_Look)
				EnhancedInputComponent->BindAction(PlayerController->Action_Look, ETriggerEvent::Triggered, this, &ATGPlayer::Look);
			if (PlayerController->Action_Jump)
				EnhancedInputComponent->BindAction(PlayerController->Action_Jump, ETriggerEvent::Triggered, this, &ATGPlayer::JumpAction);
			if (PlayerController->Action_Evade)
				EnhancedInputComponent->BindAction(PlayerController->Action_Evade, ETriggerEvent::Triggered, this, &ATGPlayer::Evade);
			if (PlayerController->Action_Build)
				EnhancedInputComponent->BindAction(PlayerController->Action_Build, ETriggerEvent::Triggered, this, &ATGPlayer::Build);
			if (PlayerController->Action_Shot)
				EnhancedInputComponent->BindAction(PlayerController->Action_Shot, ETriggerEvent::Triggered, this, &ATGPlayer::Shot);
			if (PlayerController->Action_Interact)
				EnhancedInputComponent->BindAction(PlayerController->Action_Interact, ETriggerEvent::Triggered, this, &ATGPlayer::Interact);
		}
	}
}

int32 ATGPlayer::AddPlayerHP(int32 value)
{
	HP += value;
	if (HP > MaxHP)
		HP = MaxHP;
	if (HP <= 0 && GetWorld()->GetFirstPlayerController())
	{
		DisableInput(GetWorld()->GetFirstPlayerController());
	}
	return HP;
}

void ATGPlayer::InteractiveTrace(bool debug)
{
	FHitResult HitResult;
	const bool bHit = CameraLineTrace(HitResult, ECC_GameTraceChannel1, InteractDistance);

	ATGInteractiveActor* HitActor = nullptr;
	if (bHit)
	{
		HitActor = Cast<ATGInteractiveActor>(HitResult.GetActor());
	}

	if (HitActor != CurrentFocusedActor)
	{
		if (CurrentFocusedActor)
			CurrentFocusedActor->OnUnfocused(this);

		CurrentFocusedActor = HitActor;

		if (CurrentFocusedActor)
		{
			CurrentFocusedActor->OnFocused(this);
		}
	}
}

bool ATGPlayer::CameraLineTrace(FHitResult& TraceHit, ECollisionChannel Channel, float MaxDistance, bool debug)
{
	FCollisionQueryParams QueryParams;
	TArray<AActor*> IgnoredActors = { this };
	QueryParams.AddIgnoredActors(IgnoredActors);

	const FVector Start = Camera->GetComponentLocation();
	const FVector End = Start + Camera->GetForwardVector() * MaxDistance;

	if (debug)
	{
		return UKismetSystemLibrary::LineTraceSingle(
			GetWorld(), //어느 월드의 소속인가? (this)를 넣어줘도 됨
			Start,
			End,
			UEngineTypes::ConvertToTraceType(Channel),	// 사용할 트레이스채널
			QueryParams.bTraceComplex,	// 복합콜리전 사용
			IgnoredActors,	// 해당 액터는 이 트레이스를 무시
			EDrawDebugTrace::ForOneFrame,	//디버그(그리기 타입 적용),
			TraceHit,
			true,	// 자기자신을 Ignore
			FLinearColor::Red,	//디버그 색깔
			FLinearColor::Green	//트레이스 히트 시 색깔
		);
	}
	else
	{
		return GetWorld()->LineTraceSingleByChannel(
			TraceHit,
			Start,
			End,
			Channel,
			QueryParams
		);
	}

}
