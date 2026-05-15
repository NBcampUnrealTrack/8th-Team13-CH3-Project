// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TGPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "Core/GameFlow/TGGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

ATGPlayerController::ATGPlayerController()
{
	// 플레이어 조작
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMCPlayer_Asset =
		TEXT("/Game/Player/Input/IMC_Player.IMC_Player");
	if (IMCPlayer_Asset.Succeeded())
		IMC_Player = IMCPlayer_Asset.Object;

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

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Shot_Asset =
		TEXT("/Game/Player/Input/Actions/IA_Shot.IA_Shot");
	if (IA_Shot_Asset.Succeeded())
		Action_Shot = IA_Shot_Asset.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Interact_Asset =
		TEXT("/Game/Player/Input/Actions/IA_Interact.IA_Interact");
	if (IA_Interact_Asset.Succeeded())
		Action_Interact = IA_Interact_Asset.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Switching_Asset =
		TEXT("/Game/Player/Input/Actions/IA_Switching.IA_Switching");
	if (IA_Switching_Asset.Succeeded())
		Action_Switching = IA_Switching_Asset.Object;
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_SelectTower_Asset =
		TEXT("/Game/Player/Input/Actions/IA_SelectTower.IA_BuildTowerSelect");
	if (IA_SelectTower_Asset.Succeeded())
		Action_SelectTower = IA_SelectTower_Asset.Object;

	// 메뉴 조작
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMCMenu_Asset =
		TEXT("/Game/Player/Input/IMC_Menu.IMC_Menu");
	if (IMCMenu_Asset.Succeeded())
		IMC_Menu = IMCMenu_Asset.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Pause_Asset =
		TEXT("/Game/Player/Input/Actions/IA_Pause.IA_Pause");
	if (IA_Pause_Asset.Succeeded())
		Action_Pause = IA_Pause_Asset.Object;

}

void ATGPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (IMC_Player)
			{
				Subsystem->AddMappingContext(IMC_Player, 0);
			}
		}
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
		{
			if (Action_Pause)
			{
				Action_Pause->bTriggerWhenPaused = true;
				EnhancedInputComponent->BindAction(Action_Pause, ETriggerEvent::Triggered, this, &ATGPlayerController::GamePause);
			}
		}
	}
}

void ATGPlayerController::GamePause()
{
	if (ATGGameMode* GM = Cast<ATGGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		if (GM->IsPauseGameFlow())
			GM->ResumeGameFlow();
		else
			GM->PauseGameFlow();
	}
}
