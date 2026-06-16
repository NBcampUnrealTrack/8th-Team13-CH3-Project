// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TGPlayer.h"
#include "EnhancedInputComponent.h"
#include "Player/TGPlayerController.h"
#include "Camera/CameraComponent.h"
#include "InputActionValue.h"
#include "Core/Grid/TGGridBase.h"
#include "Core/Grid/TGSingleGrid.h"
#include "Core/GameFlow/TGGameMode.h"
#include "TGInteractiveActor.h"
#include "Engine/DataTable.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ChildActorComponent.h"
#include "Enemies/TGEnemyBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapons/TGWeaponSingleShot.h"
#include "Weapons/TGWeaponShotgun.h"
#include "Weapons/TGWeaponRepeater.h"
#include "Components/TimelineComponent.h"
#include "Curves/CurveVector.h"
#include "BaseTower/TGBaseTower.h"
#include "TGNPCBase.h"

// Sets default values
ATGPlayer::ATGPlayer() : MaxHP(100), SlowRate(0.5f), DefaultWalkSpeed(0)
{

	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 건설 퀵슬롯 기본 타워 종류 (슬롯3번부터). 전체 슬롯 = [미선택, 벽, WeaponTower, DebuffTower, BuffTower]
	TowerSlotTypes = { ETGTurretType::WeaponTower, ETGTurretType::DebuffTower, ETGTurretType::BuffTower };

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(RootComponent);
	Camera->bUsePawnControlRotation = true;
	Camera->SetRelativeLocation(FVector(0, 0, 100));

	Weapon_Skeletal = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon_SKM");
	Weapon_Skeletal->SetupAttachment(Camera);
	Weapon_Skeletal->SetRelativeLocation(InitialLocation);

	Weapon_Static = CreateDefaultSubobject<UStaticMeshComponent>("Weapon_SM");
	Weapon_Static->SetupAttachment(Camera);
	Weapon_Static->SetRelativeLocation(InitialLocation);

	SwitchingWeaponTimelineComp = CreateDefaultSubobject<UTimelineComponent>("SwitchingWeapon_TLC");
	RecoilTimelineComp = CreateDefaultSubobject<UTimelineComponent>("Recoil_TLC");
	RecoilTimelineComp->SetPropertySetObject(this);

	static ConstructorHelpers::FObjectFinder<UCurveVector> CurveNoneAsset(TEXT("/Game/Weapons/Curves/CV_None.CV_None"));
	if (CurveNoneAsset.Succeeded())
		CurveVector_None = CurveNoneAsset.Object;

	// 무기 데이터테이블 기본값 — BP에서 교체 가능
	static ConstructorHelpers::FObjectFinder<UDataTable> SingleShotTableAsset(TEXT("/Game/Weapons/DT_WeaponTable_SingleShot.DT_WeaponTable_SingleShot"));
	if (SingleShotTableAsset.Succeeded())
		SingleShotWeaponTable = SingleShotTableAsset.Object;

	static ConstructorHelpers::FObjectFinder<UDataTable> ShotgunTableAsset(TEXT("/Game/Weapons/DT_WeaponTable_Shotgun.DT_WeaponTable_Shotgun"));
	if (ShotgunTableAsset.Succeeded())
		ShotgunWeaponTable = ShotgunTableAsset.Object;

	static ConstructorHelpers::FObjectFinder<UDataTable> RepeaterTableAsset(TEXT("/Game/Weapons/DT_WeaponTable_Repeater.DT_WeaponTable_Repeater"));
	if (RepeaterTableAsset.Succeeded())
		RepeaterWeaponTable = RepeaterTableAsset.Object;

	EvadeCount = 2;
	EvadeCooldown = 3.0f;
	InteractDistance = 300.f;
	CurrentFocusedActor = nullptr;
	LastFocusedEnemy = nullptr;
	CurrentFocusedNPC = nullptr;
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
	bCanSwitch = true;
	CurrentEvadeCount = EvadeCount;
	CurrentEvadeCooldown = 0.0f;
	OnEvadeChanged.Broadcast(CurrentEvadeCount, CurrentEvadeCooldown / EvadeCooldown);
	DefaultWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	DeathAnimPos = 0.f;
	bDeath = false;

	if (GetWorld()->GetFirstPlayerController())
		EnableInput(GetWorld()->GetFirstPlayerController());

	// 반동 타임라인 초기화
	CurrentRecoilLocScale = FVector::ZeroVector;
	CurrentRecoilRotScale = FRotator::ZeroRotator;
	FOnTimelineVector RecoilTL_CurLoc;
	FOnTimelineVector RecoilTL_CurRot;

	RecoilTL_CurLoc.BindUFunction(this, TEXT("OnAddRecoilWeaponOffset_Location"));
	RecoilTL_CurRot.BindUFunction(this, TEXT("OnAddRecoilWeaponOffset_Rotation"));


	RecoilTimelineComp->AddInterpVector(CurveVector_None, RecoilTL_CurLoc, NAME_None, FName("LocationCurve"));
	RecoilTimelineComp->AddInterpVector(CurveVector_None, RecoilTL_CurRot, NAME_None, FName("RotationCurve"));
	RecoilTimelineComp->SetLooping(false);

	// 무기 교체 타임라인 초기화
	FOnTimelineVector SwitchingWeaponTL_CurLoc;
	FOnTimelineVector SwitchingWeaponTL_CurRot;
	FOnTimelineEvent SwitchingWeaponTL_Finish;
	SwitchingWeaponTL_Finish.BindUFunction(this, TEXT("OnFinishSwitchingWeaponTimeline"));
	SwitchingWeaponTL_CurLoc.BindUFunction(this, TEXT("OnAddSwitchingWeaponOffset_Location"));
	SwitchingWeaponTL_CurRot.BindUFunction(this, TEXT("OnAddSwitchingWeaponOffset_Rotation"));

	// 무기 소유
	OwnWeapon(ETGWeaponTriggerType::REPEATER, TEXT("AssaultRifle"), true);
	OwnWeapon(ETGWeaponTriggerType::SHOTGUN, TEXT("Shotgun"), false);
	OwnWeapon(ETGWeaponTriggerType::SINGLE_SHOT, TEXT("SniperRifle"), false);
	OnWeaponChanged.Broadcast(GetCurrentWeapon());

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

	// UPROPERTY 참조만 끊으면 GC가 무기 오브젝트를 정리한다
	OwnedWeapons.Empty();

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

	OnEvadeChanged.Broadcast(CurrentEvadeCount, CurrentEvadeCooldown / EvadeCooldown);
	LaunchCharacter(LaunchVel, true, true);
}

void ATGPlayer::CycleBuildSlot(const FInputActionValue& InputValue)
{
	// Tab을 누를 때마다 다음 슬롯으로 순환 (미선택 → 벽 → 타워들 → 다시 미선택)
	const int32 SlotCount = GetBuildSlotCount();
	CurrentBuildSlot = (CurrentBuildSlot + 1) % SlotCount;

	// 슬롯이 바뀌면 현재 포커스를 비워 다음 틱에 새 슬롯 기준으로 프리뷰가 갱신되게 함
	if (CurrentFocusedActor)
	{
		CurrentFocusedActor->OnUnfocused(this);
		CurrentFocusedActor = nullptr;
	}

	OnBuildSlotChanged.Broadcast(CurrentBuildSlot, SlotCount);
}

ETGTurretType ATGPlayer::GetSlotTurretType() const
{
	// 슬롯 0=미선택, 1=벽, 2번부터 타워 종류
	const int32 TowerIdx = CurrentBuildSlot - 2;
	return TowerSlotTypes.IsValidIndex(TowerIdx) ? TowerSlotTypes[TowerIdx] : ETGTurretType::None;
}

void ATGPlayer::Shot(const FInputActionValue& InputValue)
{
	// [FIX-PACKAGING] GetCurrentWeapon() nullptr 체크 - 무기 없을 때 이중 역참조 크래시 방지
	UTGWeaponBase* CurWeapon = GetCurrentWeapon();
	if (!CurWeapon || !CurWeapon->GetAsset()) return;

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
	MuzzlePos = CurrentWeaponComp->GetComponentTransform().TransformPosition(CurWeapon->GetAsset()->MuzzlePos);

	if (CameraLineTrace(TraceHit, ECC_Visibility, CurrentWeaponComp->GetRelativeLocation().Length() + CurWeapon->GetAsset()->MuzzlePos.Length(), ShootDistance))
		ShootDir = (TraceHit.ImpactPoint - MuzzlePos).GetSafeNormal();	// Hit한 지점을 향해 발사
	else
		ShootDir = (Camera->GetComponentLocation() + Camera->GetForwardVector() * ShootDistance - MuzzlePos).GetSafeNormal();	// 카메라 정중앙을 향해 발사

	CurWeapon->Shoot(this, CurrentWeaponComp, MuzzlePos, ShootDir, ShootDistance, SwitchingWeaponTimelineComp->GetPlaybackPosition() > 0.f);
}

void ATGPlayer::Interact(const FInputActionValue& InputValue)
{
	// NPC 상호작용 (슬롯과 무관하게 상시)
	if (CurrentFocusedNPC)
	{
		CurrentFocusedNPC->OnInteract(this);
		return;
	}

	// 미선택(슬롯1)이거나 포커스된 건설 대상이 없으면 건설하지 않음 (사격은 별도)
	if (!IsBuildSlotActive() || !CurrentFocusedActor) return;

	if (IsWallSlot())
	{
		// 벽 슬롯: 빈 그리드 칸에 벽(베이스) 설치
		if (ATGSingleGrid* Grid = Cast<ATGSingleGrid>(CurrentFocusedActor))
		{
			Grid->OnInteract(this);
		}
	}
	else
	{
		// 타워 슬롯: 설치된 벽(BaseTower)에 해당 터렛 장착
		if (ABaseTower* BaseTower = Cast<ABaseTower>(CurrentFocusedActor))
		{
			BaseTower->SetSelectedTurretType(GetSlotTurretType());
			BaseTower->OnInteract(this);
		}
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
			bCanSwitch = true;
			}),
		SwitchWeaponDelay,
		false
	);

	int32 Idx = OwnedWeapons.IndexOfByPredicate([this](FWeaponPair element) {return SwitchingWeaponKey == element.Key; });
	if (moveDir > 0)
		Idx++;
	else
		Idx--;

	if (!OwnedWeapons.IsValidIndex(Idx))
	{
		if (moveDir > 0)
			Idx = 0;
		else
			Idx = OwnedWeapons.Num() - 1;
	}

	SwitchingWeaponTimelineComp->Play();
	SwitchingWeaponKey = OwnedWeapons[Idx].Key;
	OnWeaponChanged.Broadcast(OwnedWeapons[Idx].Value);
}

// Called every frame
void ATGPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RestoreEvadeCooldown(DeltaTime);	// 회피기동 쿨타임 회복

	// Interactive(빌드모드) / NPC(상시) 포커스 갱신 — 트레이스 1회로 통합
	UpdateFocusTraces();
	// 무기가 향하는 방향
	FHitResult WeaponTrace;

	// [FIX-PACKAGING] GetCurrentWeapon() nullptr 체크 - Tick에서 무기 없을 때 크래시 방지
	UTGWeaponBase* CurWeapon = GetCurrentWeapon();
	const float MuzzleLen = (CurWeapon && CurWeapon->GetAsset()) ? CurWeapon->GetAsset()->MuzzlePos.Length() : 0.f;
	CameraLineTrace(WeaponTrace, ECC_Visibility, Weapon_Static->GetRelativeLocation().Length() + MuzzleLen, ShootDistance);

	// Trace 결과 중 HitEnemy을 확인하여 기존 값과 다를 경우 Broadcast
	ATGEnemyBase* HitEnemy = Cast<ATGEnemyBase>(WeaponTrace.GetActor());
	if (HitEnemy != LastFocusedEnemy) {
		LastFocusedEnemy = HitEnemy;
		OnFocusedEnemyChanged.Broadcast(LastFocusedEnemy);
	}

	// 무기 트랜스폼 업데이트
	UpdateWeaponTransform();

	// 사망 애니메이션
	if (bDeath)
	{
		const float DeathAnimLength = 0.5f;
		DeathAnimPos += DeltaTime / DeathAnimLength;
		if (DeathAnimPos > 1.f)
			DeathAnimPos = 1.f;
		FRotator NewRotation = GetActorRotation();
		NewRotation.Roll = FMath::Lerp(0.f, 60.f, DeathAnimPos);
		Camera->bUsePawnControlRotation = false;
		SetActorRotation(NewRotation);
	}

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
		OnEvadeChanged.Broadcast(CurrentEvadeCount, CurrentEvadeCooldown / EvadeCooldown);
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

	// [FIX-PACKAGING] GetCurrentWeapon() nullptr 체크 - 무기 없을 때 크래시 방지 (배열은 항상 정리)
	UTGWeaponBase* CurWeaponForTransform = GetCurrentWeapon();
	if (CurWeaponForTransform && CurWeaponForTransform->GetAsset())
	{
		CurrentWeaponComp->SetRelativeLocation(InitialLocation + CurWeaponForTransform->GetAsset()->LocationOffset + TotalLocOffset);
		CurrentWeaponComp->SetRelativeRotation(FRotator::ZeroRotator + TotalRotOffset);
	}

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

void ATGPlayer::ClearFocusedEnemy()
{
	if (!LastFocusedEnemy) return;

	LastFocusedEnemy = nullptr;
	OnFocusedEnemyChanged.Broadcast(nullptr);
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
			if (PlayerController->Action_CycleBuildSlot)
				EnhancedInputComponent->BindAction(PlayerController->Action_CycleBuildSlot, ETriggerEvent::Triggered, this, &ATGPlayer::CycleBuildSlot);
			if (PlayerController->Action_Shot)
				EnhancedInputComponent->BindAction(PlayerController->Action_Shot, ETriggerEvent::Triggered, this, &ATGPlayer::Shot);
			if (PlayerController->Action_Interact)
				EnhancedInputComponent->BindAction(PlayerController->Action_Interact, ETriggerEvent::Triggered, this, &ATGPlayer::Interact);
			if (PlayerController->Action_Switching)
				EnhancedInputComponent->BindAction(PlayerController->Action_Switching, ETriggerEvent::Triggered, this, &ATGPlayer::SwitchingWeapon);
		}
	}
}

int32 ATGPlayer::ChangePlayerHP(int32 value)
{
	HP += value;
	if (HP > MaxHP)
		HP = MaxHP;
	OnPlayerHpChanged.Broadcast(HP, MaxHP);
	if (HP <= 0 && !bDeath && GetWorld()->GetFirstPlayerController())
	{
		DisableInput(GetWorld()->GetFirstPlayerController());
		Weapon_Skeletal->SetVisibility(false);
		Weapon_Static->SetVisibility(false);
		DeathAnimPos = 0.f;
		bDeath = true;
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

void ATGPlayer::ApplyMaxHpDelta(int32 Delta)
{
	if (Delta == 0) return;

	MaxHP = FMath::Max(1, MaxHP + Delta);
	// 최대치 증가분만큼 현재 체력도 회복 (감소 시에는 클램프만)
	if (Delta > 0)
	{
		HP += Delta;
	}
	HP = FMath::Clamp(HP, 0, MaxHP);
	OnPlayerHpChanged.Broadcast(HP, MaxHP);
}

void ATGPlayer::ApplyMoveSpeedMultiplier(float Ratio)
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (!MovementComponent) return;

	if (DefaultWalkSpeed <= 0)
	{
		DefaultWalkSpeed = MovementComponent->MaxWalkSpeed;
	}

	DefaultWalkSpeed *= (1.f + Ratio);
	// 슬로우 디버프 중이 아니면 즉시 반영 (디버프 해제 시 갱신된 기본속도로 복원됨)
	MovementComponent->MaxWalkSpeed = DefaultWalkSpeed;
}

void ATGPlayer::AddEvadeCount(int32 Delta)
{
	if (Delta == 0) return;

	EvadeCount = FMath::Max(0, EvadeCount + Delta);
	CurrentEvadeCount = FMath::Clamp(CurrentEvadeCount + Delta, 0, EvadeCount);

	const float CooldownRate = (EvadeCooldown > 0.f) ? (CurrentEvadeCooldown / EvadeCooldown) : 0.f;
	OnEvadeChanged.Broadcast(CurrentEvadeCount, CooldownRate);
}

void ATGPlayer::AddWeaponDamageMultiplier(float Add)
{
	WeaponDamageMultiplier = FMath::Max(0.f, WeaponDamageMultiplier + Add);
}

UTGWeaponBase* ATGPlayer::GetCurrentWeapon()
{
	// FindByPredicate는 일치 항목이 없으면 nullptr을 반환하므로 역참조 전에 검사
	const FWeaponPair* Found = OwnedWeapons.FindByPredicate([this](const FWeaponPair& element) {return CurrentWeaponKey == element.Key; });
	return Found ? Found->Value : nullptr;
}

void ATGPlayer::OnFinishSwitchingWeaponTimeline()
{
	if (SwitchingWeaponTimelineComp->GetPlaybackPosition() < SwitchingWeaponTimelineComp->GetTimelineLength())
		return;
	SwitchingWeaponTimelineComp->ReverseFromEnd();
	EquipWeapon(SwitchingWeaponKey);
}

void ATGPlayer::OnAddSwitchingWeaponOffset_Location(FVector Loc)
{
	WeaponLocationOffset.Add(Loc);
}

void ATGPlayer::OnAddSwitchingWeaponOffset_Rotation(FVector Rot)
{
	WeaponRotationOffset.Add(FRotator(Rot.X, Rot.Y, Rot.Z));
}

void ATGPlayer::OnAddRecoilWeaponOffset_Location(FVector Loc)
{
	WeaponLocationOffset.Add(Loc * CurrentRecoilLocScale);
}

void ATGPlayer::OnAddRecoilWeaponOffset_Rotation(FVector Rot)
{
	WeaponRotationOffset.Add(FRotator(Rot.X * CurrentRecoilRotScale.Pitch, Rot.Y * CurrentRecoilRotScale.Yaw, Rot.Z * CurrentRecoilRotScale.Roll));
	AddControllerPitchInput(-Rot.X * CurrentRecoilRotScale.Pitch * CurrentRecoilInputScale * GetWorld()->GetDeltaSeconds());
	AddControllerYawInput(Rot.Y * CurrentRecoilRotScale.Yaw * CurrentRecoilInputScale * GetWorld()->GetDeltaSeconds());
}

namespace
{
	// 무기 데이터테이블에서 Row를 찾아 무기 오브젝트를 생성하는 공통 헬퍼
	template<typename TWeaponClass, typename TRowType>
	TWeaponClass* CreateWeaponFromTable(UObject* Outer, UDataTable* Table, FName RowName)
	{
		if (!IsValid(Table)) return nullptr;

		TRowType* Info = Table->FindRow<TRowType>(RowName, TEXT(""));
		if (!Info) return nullptr;

		TWeaponClass* Weapon = NewObject<TWeaponClass>(Outer);
		Weapon->SetStatus(*Info);
		return Weapon;
	}
}

void ATGPlayer::OwnWeapon(ETGWeaponTriggerType TriggerType, FName RowName, bool equip)
{
	UTGWeaponBase* NewWeapon = nullptr;
	switch (TriggerType)
	{
	case ETGWeaponTriggerType::SINGLE_SHOT:
		NewWeapon = CreateWeaponFromTable<UTGWeaponSingleShot, FTGStatusWeaponSingleShot>(this, SingleShotWeaponTable, RowName);
		break;
	case ETGWeaponTriggerType::SHOTGUN:
		NewWeapon = CreateWeaponFromTable<UTGWeaponShotgun, FTGStatusWeaponShotgun>(this, ShotgunWeaponTable, RowName);
		break;
	case ETGWeaponTriggerType::REPEATER:
		NewWeapon = CreateWeaponFromTable<UTGWeaponRepeater, FTGStatusWeaponRepeater>(this, RepeaterWeaponTable, RowName);
		break;
	default:
		break;
	}

	if (NewWeapon)
		OwnedWeapons.Add(FWeaponPair(GetWeaponKey(TriggerType, RowName), NewWeapon));

	if (equip)
		EquipWeapon(GetWeaponKey(TriggerType, RowName));
}

void ATGPlayer::EquipWeapon(FString Key)
{
	CurrentWeaponKey = Key;
	SwitchingWeaponKey = Key;
	// [FIX-PACKAGING] GetCurrentWeapon() 및 GetAsset() nullptr 체크 - 무기 없을 때 이중 역참조 크래시 방지
	UTGWeaponBase* CurWeapon = GetCurrentWeapon();
	if (!CurWeapon || !CurWeapon->GetAsset()) return;
	FTGWeaponAsset AssetInfo = *CurWeapon->GetAsset();
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

	// 반동 틱 콜백은 BeginPlay에서 바인딩이 끝났으므로 여기서는 커브만 교체한다
	RecoilTimelineComp->SetVectorCurve(AssetInfo.RecoilCurveLoc.IsNull() ? CurveVector_None : AssetInfo.RecoilCurveLoc, FName("LocationCurve"));
	RecoilTimelineComp->SetVectorCurve(AssetInfo.RecoilCurveRot.IsNull() ? CurveVector_None : AssetInfo.RecoilCurveRot, FName("RotationCurve"));
}

FString ATGPlayer::GetWeaponKey(ETGWeaponTriggerType TriggerType, FName WeaponName)
{
	return FString::Printf(TEXT("%d_%s"), static_cast<int32>(TriggerType), *WeaponName.ToString());
}

void ATGPlayer::PlayRecoil(float ShotInterval, float RecoilInputScale)
{
	CurrentRecoilLocScale = FVector(FMath::RandRange(0.75f, 1.0f), FMath::RandRange(-1.0f, 1.0f), FMath::RandRange(0.75f, 1.0f));
	CurrentRecoilRotScale = FRotator(FMath::RandRange(0.75f, 1.0f), FMath::RandRange(-1.0f, 1.0f), 0.f);
	CurrentRecoilInputScale = RecoilInputScale;
	RecoilTimelineComp->SetPlayRate(1.0f / ShotInterval);
	RecoilTimelineComp->PlayFromStart();
}

void ATGPlayer::ClearSlowDebuff()
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (!MovementComponent) return;

	// 기본 이동속도로 복원
	MovementComponent->MaxWalkSpeed = DefaultWalkSpeed;
}

void ATGPlayer::UpdateFocusTraces()
{
	// Interactive/NPC가 같은 채널·거리를 쓰므로 트레이스는 한 번만 수행한다
	FHitResult HitResult;
	const bool bHit = CameraLineTrace(HitResult, ECC_GameTraceChannel1, 0.0f, InteractDistance);
	AActor* HitActor = bHit ? HitResult.GetActor() : nullptr;

	// NPC 포커스 갱신 (빌드모드와 무관하게 상시)
	CurrentFocusedNPC = Cast<ATGNPCBase>(HitActor);

	// Interactive 액터 포커스 갱신 (건설 슬롯이 활성일 때만 — 미선택이면 그리드/벽 프리뷰 없음)
	ATGInteractiveActor* HitInteractive = IsBuildSlotActive() ? Cast<ATGInteractiveActor>(HitActor) : nullptr;
	if (HitInteractive != CurrentFocusedActor)
	{
		if (CurrentFocusedActor)
			CurrentFocusedActor->OnUnfocused(this);

		CurrentFocusedActor = HitInteractive;

		if (CurrentFocusedActor)
		{
			CurrentFocusedActor->OnFocused(this);
		}
	}
}

bool ATGPlayer::CameraLineTrace(FHitResult& TraceHit, ECollisionChannel Channel, float StartDistance, float MaxDistance)
{
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	QueryParams.AddIgnoredActor(this);

	const FVector Start = Camera->GetComponentLocation() + Camera->GetForwardVector() * StartDistance;
	const FVector End = Start + Camera->GetForwardVector() * MaxDistance;

	return GetWorld()->LineTraceSingleByChannel(
		TraceHit,
		Start,
		End,
		Channel,
		QueryParams
	);
}

