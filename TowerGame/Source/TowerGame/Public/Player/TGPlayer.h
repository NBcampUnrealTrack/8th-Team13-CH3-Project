// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TGPlayer.generated.h"

struct FInputActionValue;
class ATGInteractiveActor;

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

	UFUNCTION()
	void Move(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
	UFUNCTION()
	void JumpAction(const FInputActionValue& value);
	UFUNCTION()
	void Evade(const FInputActionValue& value);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)	// BlueprintNativeEvent: C++로 짠것을 블루프린트에서 이어서 짤 수 있다.
	void Build(const FInputActionValue& InputValue);
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void RestoreEvadeCooldown(float DeltaTime);

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
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill")
	const bool GetLookingPoint(FVector& result, float MaxDistance = 5000.0f);	// 카메라가 바라보는 위치 가져오기
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill")
	const bool GetLookingPointDebug(FVector& result, float MaxDistance = 5000.0f);	// 카메라가 바라보는 위치 가져오기(시각화)

	// 현재 시선에 잡힌 Interactive 액터 반환 (없으면 nullptr)
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TowerGame|Interaction")
	ATGInteractiveActor* GetFocusedInteractiveActor() const { return CurrentFocusedActor; }

	// 상호작용 실행 (입력 바인딩에서 호출) TODO : IA 바인딩 필요
	UFUNCTION(BlueprintCallable, Category = "TowerGame|Interaction")
	void Interact();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class UCameraComponent> Camera;

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
	bool bMoving;
	FVector2D MoveDir;	// 현재 이동중인 방향, 정규화벡터

private:
	void InteractiveTrace();

	//	포커싱된 액터
	UPROPERTY()
	TObjectPtr<ATGInteractiveActor> CurrentFocusedActor;
};
