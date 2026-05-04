// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TGPlayer.h"
#include "EnhancedInputComponent.h"
#include "Player/TGPlayerController.h"
#include "Camera/CameraComponent.h"
#include "InputActionValue.h"

// Sets default values
ATGPlayer::ATGPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(RootComponent);
	Camera->bUsePawnControlRotation = true;
	EvadeCount = 2;
	EvadeCooldown = 3.0f;
}

// Called when the game starts or when spawned
void ATGPlayer::BeginPlay()
{
	Super::BeginPlay();
	bMoving = false;
	CurrentEvadeCount = EvadeCount;
	CurrentEvadeCooldown = 0.0f;
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

// Called every frame
void ATGPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RestoreEvadeCooldown(DeltaTime);	// 회피기동 쿨타임 회복
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Cyan, FString::Printf(TEXT("Current Evade Cooldown: %f / %f"), CurrentEvadeCooldown, EvadeCooldown));
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Cyan, FString::Printf(TEXT("Current Evade Count(LSHIFT): %d / %d"), CurrentEvadeCount, EvadeCount));

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
		}
	}
}

