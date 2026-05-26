// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Weapons/TGWeaponBase.h"
#include "BaseTower/TGTurretType.h"
#include "BaseTower/TGBuildWidget.h"
#include "TGPlayer.generated.h"

class ATGEnemyBase;
struct FInputActionValue;
class ATGInteractiveActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerHpChanged, float, CurrentHP, float, MaxHP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEvadeChanged, int32, EvadeCount, float, CooldownRate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFocusedEnemyChanged, ATGEnemyBase*, FocusedEnemy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTurretTypeSelected, ETGTurretType, SelectedType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponChanged, UTGWeaponBase*, WeaponAsset);

USTRUCT()
struct FWeaponPair
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FString Key;

	UPROPERTY()
	UTGWeaponBase* Value;
};

UCLASS()
class TOWERGAME_API ATGPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATGPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void Move(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
	UFUNCTION()
	void JumpAction(const FInputActionValue& value);
	UFUNCTION()
	void Evade(const FInputActionValue& value);
	UFUNCTION()
	void Build(const FInputActionValue& InputValue);
	UFUNCTION()
	void SelectTower(const FInputActionValue& InputValue);	// 숫자키로 타워 타입 선택
	UFUNCTION()
	void Shot(const FInputActionValue& InputValue);
	// 상호작용 실행 (입력 바인딩에서 호출) TODO : IA 바인딩 필요
	UFUNCTION(BlueprintCallable, Category = "TowerGame|Interaction")
	void Interact(const FInputActionValue& InputValue);
	UFUNCTION()
	void SwitchingWeapon(const FInputActionValue& InputValue);

public:
	// 델리게이트 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Enemy")
	FOnFocusedEnemyChanged OnFocusedEnemyChanged;

	UPROPERTY(BlueprintAssignable, Category = "TowerGame|Tower")
	FOnTurretTypeSelected OnTurretTypeSelected;

	FOnPlayerHpChanged OnPlayerHpChanged;
	FOnEvadeChanged OnEvadeChanged;
	FOnWeaponChanged OnWeaponChanged;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void RestoreEvadeCooldown(float DeltaTime);
	void UpdateWeaponTransform();

	virtual float TakeDamage(
		float DamageAmount, struct
		FDamageEvent const& DamageEvent, class
		AController* EventInstigator,
		AActor* DamageCauser
	) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Status")
	const int32 GetMaxEvadeCount() { return EvadeCount; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Status")
	const int32 GetCurrentEvadeCount() { return CurrentEvadeCount; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Status")
	const float GetMaxEvadeCooldown() { return EvadeCooldown; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Status")
	const float GetCurrentEvadeCooldown() { return CurrentEvadeCooldown; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Status")
	const float GetPlayerHP() { return HP; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Status")
	const float GetPlayerMaxHP() { return MaxHP; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TowerGame|Tower")
	ETGTurretType GetSelectedTurretType() const { return SelectedTurretType; }

	// 현재 시선에 잡힌 Interactive 액터 반환 (없으면 nullptr)
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TowerGame|Interaction")
	ATGInteractiveActor* GetFocusedInteractiveActor() const { return CurrentFocusedActor; }

	void ClearFocusedEnemy();

	// 기존 이름과 동작이 일치하지 않아 이름 변경 (기존 이름: AddPlayerHP)
	UFUNCTION(BlueprintCallable, Category = "Status")
	int32 ChangePlayerHP(int32 value);

	// Enemy - Debuff 적용
	UFUNCTION(BlueprintCallable, Category = "Status")
	void ApplySlowDebuff(float Duration);

	UFUNCTION(BlueprintCallable)
	UTGWeaponBase* GetCurrentWeapon();	// 현재 장착중인 무기를 가져온다.

	// 총기반동 실행
	void PlayRecoil(float ShotInterval, float RecoilInputScale);
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class UCameraComponent> Camera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> Weapon_Skeletal;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Weapon_Static;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Value")
	TArray<FVector> WeaponLocationOffset;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Value")
	TArray<FRotator> WeaponRotationOffset;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<class UTimelineComponent> RecoilTimelineComp;
	UPROPERTY(EditDefaultsOnly, Category = "Components|Value")
	TObjectPtr<UCurveVector> CurveVector_None;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<class UTimelineComponent> SwitchingWeaponTimelineComp;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<UCurveVector> SwitchingCurveLoc;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<UCurveVector> SwitchingCurveRot;

	FTimerHandle SwitchWeaponDelayHandle;
	bool bCanSwitch;


	float DeathAnimPos;
	bool bDeath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	int32 MaxHP;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	int32 HP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TowerGame|Interaction")
	float InteractDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	int32 EvadeCount;
	UPROPERTY()
	int32 CurrentEvadeCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float EvadeCooldown;
	UPROPERTY()
	float CurrentEvadeCooldown;
	float ShootDistance;
	bool bMoving;
	FVector2D MoveDir;
	FVector CurrentRecoilLocScale;
	FRotator CurrentRecoilRotScale;
	float CurrentRecoilInputScale;
	bool bBuildMode;
	ETGTurretType SelectedTurretType = ETGTurretType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower|UI")
	TSubclassOf<UTGBuildWidget> BuildWidgetClass;

	UPROPERTY()
	UTGBuildWidget* BuildWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status|Debuff")
	float SlowRate;

	UPROPERTY()
	float DefaultWalkSpeed;

	FTimerHandle SlowDebuffTimerHandle;

	UPROPERTY()
	TArray<FWeaponPair> OwnedWeapons;

	FString CurrentWeaponKey;
	FString SwitchingWeaponKey;

	UFUNCTION()
	void OnFinishSwitchingWeaponTimeline();
	UFUNCTION()
	void OnAddSwitchingWeaponOffset_Location(FVector Loc);
	UFUNCTION()
	void OnAddSwitchingWeaponOffset_Rotation(FVector Rot);
	UFUNCTION()
	void OnAddRecoilWeaponOffset_Location(FVector Loc);
	UFUNCTION()
	void OnAddRecoilWeaponOffset_Rotation(FVector Rot);

	void OwnWeapon(ETGWeaponTriggerType TriggerType, FName RowName, bool equip);
	void EquipWeapon(FString Key);
	FString GetWeaponKey(ETGWeaponTriggerType TriggerType, FName WeaponName);


	void ClearSlowDebuff();
private:
	void InteractiveTrace(bool debug = false);
	bool CameraLineTrace(FHitResult& TraceHit, ECollisionChannel Channel, float StartDistance = 0.0f, float MaxDistance = 5000.0f, bool debug = false);

	UPROPERTY()
	TObjectPtr<ATGInteractiveActor> CurrentFocusedActor;
	const FVector InitialLocation = FVector(25.0f, 25.0f, -25.0f);


	UPROPERTY()
	TObjectPtr<ATGEnemyBase> LastFocusedEnemy;

	// 마지막으로 상호작용한 액터 (대화 중 E키 처리용)
	UPROPERTY()
	TObjectPtr<ATGInteractiveActor> LastInteractedActor;
};
