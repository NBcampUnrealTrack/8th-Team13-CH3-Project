// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TGPlayer.h"
#include "EnhancedInputComponent.h"
#include "Player/TGPlayerController.h"
#include "Camera/CameraComponent.h"
#include "InputActionValue.h"
#include "Core/Grid/TGGridBase.h"
#include "Core/GameFlow/TGGameMode.h"
#include "TGInteractiveActor.h"
//#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ChildActorComponent.h"
#include "Enemies/TGEnemyBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapons/TGWeaponSingleShot.h"
#include "Weapons/TGWeaponShotgun.h"
#include "Weapons/TGWeaponRepeater.h"
#include "Curves/CurveVector.h"
#include "BaseTower/TGBaseTower.h"

// Sets default values
ATGPlayer::ATGPlayer() : MaxHP(100), SlowRate(0.5f), DefaultWalkSpeed(0)
{

	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(RootComponent);
	Camera->bUsePawnControlRotation = true;

	Weapon_Skeletal = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon_SKM");
	Weapon_Skeletal->SetupAttachment(Camera);
	Weapon_Skeletal->SetRelativeLocation(InitialLocation);

	Weapon_Static = CreateDefaultSubobject<UStaticMeshComponent>("Weapon_SM");
	Weapon_Static->SetupAttachment(Camera);
	Weapon_Static->SetRelativeLocation(InitialLocation);

	SwitchingWeaponTimelineComp = CreateDefaultSubobject<UTimelineComponent>("SwitchingWeapon_TLC");
	SwitchingWeaponTL_Finish.BindUFunction(this, TEXT("OnFinishSwitchingWeaponTimeline"));
	SwitchingWeaponTL_CurLoc.BindUFunction(this, TEXT("OnUpdateSwitchingWeaponTimeline_Location"));
	SwitchingWeaponTL_CurRot.BindUFunction(this, TEXT("OnUpdateSwitchingWeaponTimeline_Rotation"));

	EvadeCount = 2;
	EvadeCooldown = 3.0f;
	InteractDistance = 300.f;
	CurrentFocusedActor = nullptr;
	LastFocusedEnemy = nullptr;
	ShootDistance = 50000.f;
}

// Called when the game starts or when spawned
void ATGPlayer::BeginPlay()
{
	Super::BeginPlay();
	Weapon_Skeletal->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Weapon_Static->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	HP = MaxHP;
	bMoving = false;
	bBuildMode = false;
	bCanSwitch = true;
	CurrentEvadeCount = EvadeCount;
	CurrentEvadeCooldown = 0.0f;
	DefaultWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;

	if (GetWorld()->GetFirstPlayerController())
		EnableInput(GetWorld()->GetFirstPlayerController());

	OwnWeapon(ETGWeaponTriggerType::REPEATER, TEXT("AssaultRifle"), true);
	OwnWeapon(ETGWeaponTriggerType::SHOTGUN, TEXT("Shotgun"), false);
	OwnWeapon(ETGWeaponTriggerType::SINGLE_SHOT, TEXT("SniperRifle"), false);

	if (!SwitchingCurveLoc.IsNull())
		SwitchingWeaponTimelineComp->AddInterpVector(SwitchingCurveLoc, SwitchingWeaponTL_CurLoc);
	if (!SwitchingCurveRot.IsNull())
		SwitchingWeaponTimelineComp->AddInterpVector(SwitchingCurveRot, SwitchingWeaponTL_CurRot);
	SwitchingWeaponTimelineComp->SetTimelineFinishedFunc(SwitchingWeaponTL_Finish);
	SwitchingWeaponTimelineComp->SetLooping(false);
}

void ATGPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Safety check to stop the timer from executing after the actor is gone
	if (GetWorld() && GetWorld()->GetTimerManager().IsTimerActive(SwitchWeaponDelayHandle))
		GetWorld()->GetTimerManager().ClearTimer(SwitchWeaponDelayHandle);
	if (GetWorld() && GetWorld()->GetTimerManager().IsTimerActive(SlowDebuffTimerHandle))
		GetWorld()->GetTimerManager().ClearTimer(SlowDebuffTimerHandle);

	Super::EndPlay(EndPlayReason);
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
	if (!bWasJumping)
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

	// 빌드모드 GameMode 및 TGHUD에서 관리되게 변경함
	if (ATGGameMode* GM = Cast<ATGGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (bBuildMode)
		{
			GM->EnterBuildMode();
		}
		else
		{
			GM->ExitBuildMode();
		}
	}
}

void ATGPlayer::SelectTower(const FInputActionValue& InputValue)
{
	// 빌드모드일 때만 타워 선택 가능
	if (!bBuildMode) return;

	int32 SlotIndex = FMath::RoundToInt(InputValue.Get<float>());
	switch (SlotIndex)
	{
	//case 1: SelectedTurretType = ETGTurretType::BaseTower;   break;	// 1번 : BaseTower
	case 2: SelectedTurretType = ETGTurretType::WeaponTower; break;	// 2번 : WeaponTower
	case 3: SelectedTurretType = ETGTurretType::DebuffTower; break;	// 3번 : DebuffTower
	case 4: SelectedTurretType = ETGTurretType::BuffTower;   break;	// 4번 : BuffTower
	default: break;
	}

	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow,
	//	FString::Printf(TEXT("[SelectTower] Slot %d → %s"), SlotIndex, *UEnum::GetValueAsString(SelectedTurretType)));
}

void ATGPlayer::Shot(const FInputActionValue& InputValue)
{
	FHitResult TraceHit;
	FVector MuzzlePos;
	FVector ShootDir;
	UMeshComponent* CurrentWeaponComp = nullptr;
	if (Weapon_Skeletal->GetSkeletalMeshAsset())
		CurrentWeaponComp = Weapon_Skeletal;
	else if (Weapon_Static->GetStaticMesh())
		CurrentWeaponComp = Weapon_Static;
	else
		CurrentWeaponComp = GetMesh();
	MuzzlePos = CurrentWeaponComp->GetComponentTransform().TransformPosition(GetCurrentWeapon()->GetAsset()->MuzzlePos);

	if (CameraLineTrace(TraceHit, ECC_Visibility, CurrentWeaponComp->GetRelativeLocation().Length() + GetCurrentWeapon()->GetAsset()->MuzzlePos.Length(), ShootDistance))
		ShootDir = (TraceHit.ImpactPoint - MuzzlePos).GetSafeNormal();	// Hit한 지점을 향해 발사
	else
		ShootDir = (Camera->GetComponentLocation() + Camera->GetForwardVector() * ShootDistance - MuzzlePos).GetSafeNormal();	// 카메라 정중앙을 향해 발사

	GetCurrentWeapon()->Shoot(this, CurrentWeaponComp, MuzzlePos, ShootDir, ShootDistance, SwitchingWeaponTimelineComp->GetPlaybackPosition() > 0.f);
}

void ATGPlayer::Interact(const FInputActionValue& InputValue)
{
	if (bBuildMode && CurrentFocusedActor)
	{
		// 선택된 타워 타입을 BaseTower에 전달 후 상호작용
		if (ABaseTower* BaseTower = Cast<ABaseTower>(CurrentFocusedActor))
		{
			BaseTower->SetSelectedTurretType(SelectedTurretType);
		}
		CurrentFocusedActor->OnInteract(this);
	}
}

void ATGPlayer::SwitchingWeapon(const FInputActionValue& InputValue)
{
	const float SwitchWeaponDelay = 0.1f;
	const float moveDir = InputValue.Get<float>();
	if (FMath::IsNearlyZero(moveDir) || !bCanSwitch)
		return;

	bCanSwitch = false;
	GetWorld()->GetTimerManager().SetTimer(
		SwitchWeaponDelayHandle,
		FTimerDelegate::CreateLambda([this]() {
			GetWorld()->GetTimerManager().ClearTimer(SlowDebuffTimerHandle);
			bCanSwitch = true;
		}),
		SwitchWeaponDelay,
		false
	);
	TArray<TPair<FString, TObjectPtr<UTGWeaponBase>>> Arr = OwnedWeapons.Array();
	int32 Idx = Arr.IndexOfByPredicate([this](TPair<FString, TObjectPtr<UTGWeaponBase>> element) {return SwitchingWeaponKey == element.Key; });
	if (moveDir > 0)
		Idx++;
	else
		Idx--;

	if (!Arr.IsValidIndex(Idx))
	{
		if (moveDir > 0)
			Idx = 0;
		else
			Idx = Arr.Num() - 1;
	}

	SwitchingWeaponTimelineComp->Play();
	SwitchingWeaponKey = Arr[Idx].Key;
}

// Called every frame
void ATGPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RestoreEvadeCooldown(DeltaTime);	// 회피기동 쿨타임 회복

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

	}
	// 무기가 향하는 방향
	FHitResult WeaponTrace;

	CameraLineTrace(WeaponTrace, ECC_Visibility, Weapon_Static->GetRelativeLocation().Length() + GetCurrentWeapon()->GetAsset()->MuzzlePos.Length(), ShootDistance);

	//FVector MuzzlePos = Weapon_Static->GetComponentTransform().TransformPosition(GetCurrentWeapon()->GetAsset()->MuzzlePos);
	//Weapon_Static->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(Camera->GetComponentLocation() + InitialLocation + GetCurrentWeapon()->GetAsset()->LocationOffset, WeaponTrace.Location));
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Cyan, FString::Printf(TEXT("Aim Rot: %s"), *Weapon_Static->GetComponentRotation().ToString()));

	// Trace 결과 중 HitEnemy을 확인하여 기존 값과 다를 경우 Broadcast
	ATGEnemyBase* HitEnemy = Cast<ATGEnemyBase>(WeaponTrace.GetActor());
	if (HitEnemy != LastFocusedEnemy) {
		LastFocusedEnemy = HitEnemy;
		OnFocusedEnemyChanged.Broadcast(LastFocusedEnemy);
	}

	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Cyan, FString::Printf(TEXT("Current Evade Cooldown: %f / %f"), CurrentEvadeCooldown, EvadeCooldown));
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Cyan, FString::Printf(TEXT("Current Evade Count(LSHIFT): %d / %d"), CurrentEvadeCount, EvadeCount));
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Cyan, FString::Printf(TEXT("Aim Target: %s"), WeaponTrace.bBlockingHit ? *WeaponTrace.GetActor()->GetName() : TEXT("None")));
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, bBuildMode ? FColor::Emerald : FColor::Orange, FString::Printf(TEXT("Current Mode: %s"), bBuildMode ? TEXT("Build") : TEXT("Combat")));

	// 무기 트랜스폼 업데이트
	UpdateWeaponTransform();

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

void ATGPlayer::UpdateWeaponTransform()
{
	UMeshComponent* CurrentWeaponComp = nullptr;
	FVector TotalLocOffset = FVector::ZeroVector;
	FRotator TotalRotOffset = FRotator::ZeroRotator;
	if (Weapon_Skeletal->GetSkeletalMeshAsset())
		CurrentWeaponComp = Weapon_Skeletal;
	else if (Weapon_Static->GetStaticMesh())
		CurrentWeaponComp = Weapon_Static;
	else
		return;

	for (FVector loc : WeaponLocationOffset)
		TotalLocOffset += loc;
	for (FRotator rot : WeaponRotationOffset)
		TotalRotOffset += rot;
	
	CurrentWeaponComp->SetRelativeLocation(InitialLocation + GetCurrentWeapon()->GetAsset()->LocationOffset + TotalLocOffset);
	CurrentWeaponComp->SetRelativeRotation(FRotator::ZeroRotator + TotalRotOffset);

	WeaponLocationOffset.Empty();
	WeaponRotationOffset.Empty();
}

float ATGPlayer::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	const float AppliedDamage =
		Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	ChangePlayerHP(-FMath::RoundToInt(AppliedDamage));

	return AppliedDamage;
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
			if (PlayerController->Action_Switching)
				EnhancedInputComponent->BindAction(PlayerController->Action_Switching, ETriggerEvent::Triggered, this, &ATGPlayer::SwitchingWeapon);
			if (PlayerController->Action_SelectTower)
				EnhancedInputComponent->BindAction(PlayerController->Action_SelectTower, ETriggerEvent::Triggered, this, &ATGPlayer::SelectTower);
		}
	}
}

int32 ATGPlayer::ChangePlayerHP(int32 value)
{
	HP += value;
	if (HP > MaxHP)
		HP = MaxHP;
	if (HP <= 0 && GetWorld()->GetFirstPlayerController())
	{
		DisableInput(GetWorld()->GetFirstPlayerController());
		if (ATGGameMode* GM = Cast<ATGGameMode>(GetWorld()->GetAuthGameMode()))
		{
			//	TODO : 게임 오버 처리
			GM->HandleGameOver();
		}
	}
	return HP;
}

void ATGPlayer::ApplySlowDebuff(float Duration)
{
	if (Duration <= 0) return;

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (!MovementComponent) return;

	if (DefaultWalkSpeed <= 0) {
		DefaultWalkSpeed = MovementComponent->MaxWalkSpeed;
	}

	// Slow 효과 적용
	MovementComponent->MaxWalkSpeed = DefaultWalkSpeed * SlowRate;

	// Slow효과를 해제할 Timer 추가
	UWorld* World = GetWorld();
	if (!World) return;

	World->GetTimerManager().ClearTimer(SlowDebuffTimerHandle);
	World->GetTimerManager().SetTimer(
		SlowDebuffTimerHandle,
		this,
		&ATGPlayer::ClearSlowDebuff,
		Duration,
		false
	);
}

UTGWeaponBase* ATGPlayer::GetCurrentWeapon()
{
	return OwnedWeapons.Find(CurrentWeaponKey)->Get();
}

void ATGPlayer::OnFinishSwitchingWeaponTimeline()
{
	if (SwitchingWeaponTimelineComp->GetPlaybackPosition() < SwitchingWeaponTimelineComp->GetTimelineLength())
		return;
	SwitchingWeaponTimelineComp->ReverseFromEnd();
	EquipWeapon(SwitchingWeaponKey);
}

void ATGPlayer::OnUpdateSwitchingWeaponTimeline_Location(FVector Loc)
{
	WeaponLocationOffset.Add(Loc);
}

void ATGPlayer::OnUpdateSwitchingWeaponTimeline_Rotation(FVector Rot)
{
	WeaponRotationOffset.Add(FRotator(Rot.Y, Rot.Z, Rot.X));
}

void ATGPlayer::OwnWeapon(ETGWeaponTriggerType TriggerType, FName RowName, bool equip)
{
	UDataTable* DT;
	switch (TriggerType)
	{
	case ETGWeaponTriggerType::SINGLE_SHOT:
	{
		UTGWeaponSingleShot* SingleShot;
		FTGStatusWeaponSingleShot* info;
		DT = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr,
			TEXT("/Game/Weapons/DT_WeaponTable_SingleShot.DT_WeaponTable_SingleShot")));
		if (DT->IsValidLowLevel())
		{
			TArray<FName> DTNameArray = DT->GetRowNames();
			info = DT->FindRow<FTGStatusWeaponSingleShot>(RowName, TEXT(""));
			if (info)
			{
				SingleShot = NewObject<UTGWeaponSingleShot>(this);
				SingleShot->SetStatus(*info);
				OwnedWeapons.Add(GetWeaponKey(TriggerType, RowName), SingleShot);
			}
		}
		break;
	}
	case ETGWeaponTriggerType::SHOTGUN:
	{
		UTGWeaponShotgun* Shotgun;
		FTGStatusWeaponShotgun* info;
		DT = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr,
			TEXT("/Game/Weapons/DT_WeaponTable_Shotgun.DT_WeaponTable_Shotgun")));
		if (DT->IsValidLowLevel())
		{
			TArray<FName> DTNameArray = DT->GetRowNames();
			info = DT->FindRow<FTGStatusWeaponShotgun>(RowName, TEXT(""));
			if (info)
			{
				Shotgun = NewObject<UTGWeaponShotgun>(this);
				Shotgun->SetStatus(*info);
				OwnedWeapons.Add(GetWeaponKey(TriggerType, RowName), Shotgun);
			}
		}
		break;
	}
	case ETGWeaponTriggerType::REPEATER:
	{
		UTGWeaponRepeater* Repeater;
		FTGStatusWeaponRepeater* info;
		DT = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr,
			TEXT("/Game/Weapons/DT_WeaponTable_Repeater.DT_WeaponTable_Repeater")));
		if (DT->IsValidLowLevel())
		{
			TArray<FName> DTNameArray = DT->GetRowNames();
			info = DT->FindRow<FTGStatusWeaponRepeater>(RowName, TEXT(""));
			if (info)
			{
				Repeater = NewObject<UTGWeaponRepeater>(this);
				Repeater->SetStatus(*info);
				OwnedWeapons.Add(GetWeaponKey(TriggerType, RowName), Repeater);
			}
		}
		break;
	}
	}

	if (equip)
		EquipWeapon(GetWeaponKey(TriggerType, RowName));
}

void ATGPlayer::EquipWeapon(FString Key)
{
	CurrentWeaponKey = Key;
	SwitchingWeaponKey = Key;
	FTGWeaponAsset AssetInfo = *GetCurrentWeapon()->GetAsset();
	if (AssetInfo.SkeletalMesh)
	{
		Weapon_Static->SetStaticMesh(nullptr);
		Weapon_Skeletal->SetSkeletalMeshAsset(AssetInfo.SkeletalMesh);
	}
	else if (AssetInfo.StaticMesh)
	{
		Weapon_Skeletal->SetSkeletalMeshAsset(nullptr);
		Weapon_Static->SetStaticMesh(AssetInfo.StaticMesh);
	}
}

FString ATGPlayer::GetWeaponKey(ETGWeaponTriggerType TriggerType, FName WeaponName)
{
	return FString::Printf(TEXT("%d_%s"), TriggerType, *WeaponName.ToString());
}

void ATGPlayer::ClearSlowDebuff()
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (!MovementComponent) return;

	// 기본 이동속도로 복원
	MovementComponent->MaxWalkSpeed = DefaultWalkSpeed;
}

void ATGPlayer::InteractiveTrace(bool debug)
{
	FHitResult HitResult;
	const bool bHit = CameraLineTrace(HitResult, ECC_GameTraceChannel1, 0.0f, InteractDistance);

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

bool ATGPlayer::CameraLineTrace(FHitResult& TraceHit, ECollisionChannel Channel, float StartDistance, float MaxDistance, bool debug)
{
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	TArray<AActor*> IgnoredActors = { this };
	QueryParams.AddIgnoredActors(IgnoredActors);

	const FVector Start = Camera->GetComponentLocation() + Camera->GetForwardVector() * StartDistance;
	const FVector End = Start + Camera->GetForwardVector() * MaxDistance;

	if (debug)
	{
		//return UKismetSystemLibrary::LineTraceSingle(
		//	GetWorld(), //어느 월드의 소속인가? (this)를 넣어줘도 됨
		//	Start,
		//	End,
		//	UEngineTypes::ConvertToTraceType(Channel),	// 사용할 트레이스채널
		//	QueryParams.bTraceComplex,	// 복합콜리전 사용
		//	IgnoredActors,	// 해당 액터는 이 트레이스를 무시
		//	EDrawDebugTrace::ForOneFrame,	//디버그(그리기 타입 적용),
		//	TraceHit,
		//	true,	// 자기자신을 Ignore
		//	FLinearColor::Red,	//디버그 색깔
		//	FLinearColor::Green	//트레이스 히트 시 색깔
		//);
		return false;
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
