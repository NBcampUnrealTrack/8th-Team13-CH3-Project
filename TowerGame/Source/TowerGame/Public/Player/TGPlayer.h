// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TGPlayer.generated.h"

struct FInputActionValue;

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
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class UCameraComponent> Camera;
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
};
