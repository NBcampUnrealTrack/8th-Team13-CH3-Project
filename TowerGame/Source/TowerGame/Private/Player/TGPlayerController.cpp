// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TGPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

ATGPlayerController::ATGPlayerController()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC_Asset =
		TEXT("/Game/Player/Input/IMC_Player.IMC_Player");
	if (IMC_Asset.Succeeded())
		InputMappingContext = IMC_Asset.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Move_Asset =
		TEXT("/Game/Player/Input/Actions/IA_Move.IA_Move");
	if (IA_Move_Asset.Succeeded())
		Action_Move = IA_Move_Asset.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Look_Asset =
		TEXT("/Game/Player/Input/Actions/IA_Look.IA_Look");
	if (IA_Look_Asset.Succeeded())
		Action_Look = IA_Look_Asset.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Jump_Asset =
		TEXT("/Game/Player/Input/Actions/IA_Jump.IA_Jump");
	if (IA_Jump_Asset.Succeeded())
		Action_Jump = IA_Jump_Asset.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Evade_Asset =
		TEXT("/Game/Player/Input/Actions/IA_Evade.IA_Evade");
	if (IA_Evade_Asset.Succeeded())
		Action_Evade = IA_Evade_Asset.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Build_Asset =
		TEXT("/Game/Player/Input/Actions/IA_Build.IA_Build");
	if (IA_Build_Asset.Succeeded())
		Action_Build = IA_Build_Asset.Object;
}

void ATGPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}
