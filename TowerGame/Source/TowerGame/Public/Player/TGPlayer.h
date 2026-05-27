// Fill out your copyright notice in the Description page of Project Settings.

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
	const int32 GetMaxEvadeCount() { return EvadeCount; }	// 최대 Evade가능 횟수
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Status")
	const int32 GetCurrentEvadeCount() { return CurrentEvadeCount; }	// 현재 Evade 가능 횟수
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Status")
	const float GetMaxEvadeCooldown() { return EvadeCooldown; }	// 최대 Evade 쿨타임
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Status")
	const float GetCurrentEvadeCooldown() { return CurrentEvadeCooldown; }	// 현재 남은 Evade 쿨타임
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Status")
	const float GetPlayerHP() { return HP; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Status")
	const float GetPlayerMaxHP() { return MaxHP; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TowerGame|Tower")
	ETGTurretType GetSelectedTurretType() const { return SelectedTurretType; }
	//UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill")
	//const bool GetLookingPoint(FVector& result, float MaxDistance = 5000.0f);	// 카메라가 바라보는 위치 가져오기
	//UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill")
	//const bool GetLookingPointDebug(FVector& result, float MaxDistance = 5000.0f);	// 카메라가 바라보는 위치 가져오기(시각화)

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
	TArray<FVector> WeaponLocationOffset;	// 로케이션 오프셋. 실제 트랜스폼은 기본값에 이 오프셋들을 합쳐서 업데이트함.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Value")
	TArray<FRotator> WeaponRotationOffset;	// 회전의 목표값
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

	// 사망 시의 애니메이션 현재 재생위치 (0~1)
	float DeathAnimPos;
	bool bDeath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	int32 MaxHP;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	int32 HP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TowerGame|Interaction")
	float InteractDistance;	//	상호작용 최대 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	int32 EvadeCount;		// 회피기동 가능 횟수
	UPROPERTY()
	int32 CurrentEvadeCount;	// 현재 남은 회피기동 횟수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float EvadeCooldown;	// 회피기동 회복시간
	UPROPERTY()
	float CurrentEvadeCooldown;	// 현재 남은 회피기동 회복시간
	float ShootDistance;
	bool bMoving;
	FVector2D MoveDir;	// 현재 이동중인 방향, 정규화벡터
	FVector CurrentRecoilLocScale;
	FRotator CurrentRecoilRotScale;
	float CurrentRecoilInputScale;
	bool bBuildMode;	// 빌드모드
	ETGTurretType SelectedTurretType = ETGTurretType::None;	// 숫자키로 선택한 타워 타입

	// 빌드모드 진입 시 표시할 위젯 클래스 — 에디터에서 WBP_BuildWidget 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower|UI")
	TSubclassOf<UTGBuildWidget> BuildWidgetClass;

	// 런타임 위젯 인스턴스
	UPROPERTY()
	UTGBuildWidget* BuildWidget;

	// Debuff - Slow 관련 변수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status|Debuff")
	float SlowRate;

	UPROPERTY()
	float DefaultWalkSpeed;

	FTimerHandle SlowDebuffTimerHandle;

	UPROPERTY()
	TArray<FWeaponPair> OwnedWeapons;	// 소유중인 무기

	FString CurrentWeaponKey;	// 현재 장착중인 무기의 Key
	FString SwitchingWeaponKey;	// 교체중인 무기의 Key

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

	void OwnWeapon(ETGWeaponTriggerType TriggerType, FName RowName, bool equip);	// 무기를 소유한다. equip을 하면 장착까지 한다.
	void EquipWeapon(FString Key);	// 무기를 장착한다.
	FString GetWeaponKey(ETGWeaponTriggerType TriggerType, FName WeaponName);

	// Debuff - Slow 해제
	void ClearSlowDebuff();
private:
	void InteractiveTrace(bool debug = false);
	bool CameraLineTrace(FHitResult& TraceHit, ECollisionChannel Channel, float StartDistance = 0.0f, float MaxDistance = 5000.0f, bool debug = false);


	//	포커싱된 액터
	UPROPERTY()
	TObjectPtr<ATGInteractiveActor> CurrentFocusedActor;
	const FVector InitialLocation = FVector(25.0f, 25.0f, -25.0f);

	// 감지한 Enemy
	UPROPERTY()
	TObjectPtr<ATGEnemyBase> LastFocusedEnemy;
};
