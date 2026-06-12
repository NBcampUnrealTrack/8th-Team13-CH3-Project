// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TGInteractiveActor.generated.h"

class ATGPlayer;

UCLASS(Abstract)
class TOWERGAME_API ATGInteractiveActor : public AActor
{
	GENERATED_BODY()

public:
	ATGInteractiveActor();

protected:
	virtual void BeginPlay() override;

public:
	// 플레이어 시선이 닿을 때
	UFUNCTION(BlueprintNativeEvent, Category = "TowerGame|Interaction")
	void OnFocused(ATGPlayer* Player);
	virtual void OnFocused_Implementation(ATGPlayer* Player);

	// 플레이어 시선이 벗어날 때
	UFUNCTION(BlueprintNativeEvent, Category = "TowerGame|Interaction")
	void OnUnfocused(ATGPlayer* Player);
	virtual void OnUnfocused_Implementation(ATGPlayer* Player);

	// 플레이어가 상호작용 키를 눌렀을 때
	UFUNCTION(BlueprintNativeEvent, Category = "TowerGame|Interaction")
	void OnInteract(ATGPlayer* Player);
	virtual void OnInteract_Implementation(ATGPlayer* Player);

	//	상호작용 토글 함수
	UFUNCTION(BlueprintCallable, Category = "TowerGame|Interaction")
	void SetInteractionEnabled(bool bEnabled);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TowerGame|Interaction")
	bool IsInteractionEnabled() const { return bInteractionEnabled; }

protected:
	// Interactive 트레이스 채널에만 반응하는 콜리전
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TowerGame|Interaction")
	TObjectPtr<class UBoxComponent> InteractionCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TowerGame|Interaction")
	FVector InteractionBoxExtent;

private:
	bool bInteractionEnabled = true;
};
