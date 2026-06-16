// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Weapons/TGWeaponBase.h"
#include "BaseTower/TGTurretType.h"
#include "TGPlayer.generated.h"

class ATGEnemyBase;
struct FInputActionValue;
class ATGInteractiveActor;
class UDataTable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerHpChanged, float, CurrentHP, float, MaxHP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEvadeChanged, int32, EvadeCount, float, CooldownRate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFocusedEnemyChanged, ATGEnemyBase*, FocusedEnemy);
// 빌드 퀵슬롯 변경 알림 (SelectedIndex: 현재 슬롯, SlotCount: 전체 슬롯 수)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBuildSlotChanged, int32, SelectedIndex, int32, SlotCount);
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
	void CycleBuildSlot(const FInputActionValue& InputValue);	// Tab으로 건설 퀵슬롯 순환
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

	UPROPERTY(BlueprintAssignable, Category = "TowerGame|Build")
	FOnBuildSlotChanged OnBuildSlotChanged;

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
	int32 GetMaxEvadeCount() const { return EvadeCount; }	// 최대 Evade가능 횟수
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Status")
	int32 GetCurrentEvadeCount() const { return CurrentEvadeCount; }	// 현재 Evade 가능 횟수
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Status")
	float GetMaxEvadeCooldown() const { return EvadeCooldown; }	// 최대 Evade 쿨타임
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Status")
	float GetCurrentEvadeCooldown() const { return CurrentEvadeCooldown; }	// 현재 남은 Evade 쿨타임
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Status")
	float GetPlayerHP() const { return HP; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Status")
	float GetPlayerMaxHP() const { return MaxHP; }

	// ───────── 건설 퀵슬롯 ─────────
	// 현재 선택 슬롯 (0=미선택, 1=벽, 2.. = TowerSlotTypes[idx-2])
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TowerGame|Build")
	int32 GetCurrentBuildSlot() const { return CurrentBuildSlot; }
	// 전체 슬롯 수 (미선택 + 벽 + 타워 종류)
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TowerGame|Build")
	int32 GetBuildSlotCount() const { return 2 + TowerSlotTypes.Num(); }
	// 건설 슬롯(벽/타워)이 활성인지 (0=미선택이면 false)
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TowerGame|Build")
	bool IsBuildSlotActive() const { return CurrentBuildSlot >= 1; }
	// 현재 슬롯이 벽 슬롯인지 (1번)
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TowerGame|Build")
	bool IsWallSlot() const { return CurrentBuildSlot == 1; }
	// 현재 슬롯이 가리키는 타워 종류 (타워 슬롯이 아니면 None)
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TowerGame|Build")
	ETGTurretType GetSlotTurretType() const;

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

	// ───────── 특성(Perk) 적용 API ─────────
	// 최대 체력 증가 (증가분만큼 현재 체력도 함께 회복)
	UFUNCTION(BlueprintCallable, Category = "Status")
	void ApplyMaxHpDelta(int32 Delta);
	// 이동 속도 증가 (Ratio 비율만큼, 0.1 = +10%)
	UFUNCTION(BlueprintCallable, Category = "Status")
	void ApplyMoveSpeedMultiplier(float Ratio);
	// 회피 가능 횟수 증가
	UFUNCTION(BlueprintCallable, Category = "Status")
	void AddEvadeCount(int32 Delta);
	// 무기 데미지 배수 증가 (Add만큼 가산, 0.15 = +15%)
	UFUNCTION(BlueprintCallable, Category = "Status")
	void AddWeaponDamageMultiplier(float Add);
	// 현재 무기 데미지 배수 (무기 발사 시 데미지에 곱해짐)
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Status")
	float GetWeaponDamageMultiplier() const { return WeaponDamageMultiplier; }

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

	// 무기 데미지 배수 (특성으로 증가, 기본 1.0)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	float WeaponDamageMultiplier = 1.f;

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
	// 퀵슬롯 3번부터 노출할 타워 종류 순서 (BP에서 조정). 기본값은 생성자에서 설정.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TowerGame|Build")
	TArray<ETGTurretType> TowerSlotTypes;
	// 현재 선택된 건설 퀵슬롯 인덱스 (0=미선택, 1=벽, 2.. = 타워)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TowerGame|Build")
	int32 CurrentBuildSlot = 0;

	// Debuff - Slow 관련 변수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status|Debuff")
	float SlowRate;

	UPROPERTY()
	float DefaultWalkSpeed;

	FTimerHandle SlowDebuffTimerHandle;

	UPROPERTY()
	TArray<FWeaponPair> OwnedWeapons;	// 소유중인 무기

	// 무기 데이터테이블 — 생성자에서 기본 에셋이 지정되며, BP에서 교체할 수 있다
	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TObjectPtr<UDataTable> SingleShotWeaponTable;
	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TObjectPtr<UDataTable> ShotgunWeaponTable;
	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TObjectPtr<UDataTable> RepeaterWeaponTable;

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
	// Interactive/NPC 포커스 갱신 — 같은 채널을 쓰므로 트레이스는 한 번만 수행
	void UpdateFocusTraces();
	bool CameraLineTrace(FHitResult& TraceHit, ECollisionChannel Channel, float StartDistance = 0.0f, float MaxDistance = 5000.0f);


	//	포커싱된 액터
	UPROPERTY()
	TObjectPtr<ATGInteractiveActor> CurrentFocusedActor;
	const FVector InitialLocation = FVector(25.0f, 25.0f, -25.0f);

	// 감지한 Enemy
	UPROPERTY()
	TObjectPtr<ATGEnemyBase> LastFocusedEnemy;

	// NPC 전용 - 감지한 NPC
	UPROPERTY()
	TObjectPtr<ATGInteractiveActor> CurrentFocusedNPC;
};
