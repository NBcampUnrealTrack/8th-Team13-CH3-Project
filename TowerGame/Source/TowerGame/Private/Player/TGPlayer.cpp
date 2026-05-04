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
}

// Called when the game starts or when spawned
void ATGPlayer::BeginPlay()
{
	Super::BeginPlay();

}

void ATGPlayer::Move(const FInputActionValue& value)
{
	if (!Controller) return;
	FVector2D MoveInput = value.Get<FVector2D>();
	if (!FMath::IsNearlyZero(MoveInput.Y))
		AddMovementInput(GetActorForwardVector(), MoveInput.Y);
	if (!FMath::IsNearlyZero(MoveInput.X))
		AddMovementInput(GetActorRightVector(), MoveInput.X);
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

// Called every frame
void ATGPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
		}
	}
}

