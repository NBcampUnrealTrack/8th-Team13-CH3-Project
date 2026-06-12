// Fill out your copyright notice in the Description page of Project Settings.

#include "TGInteractiveActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

ATGInteractiveActor::ATGInteractiveActor()
	: InteractionBoxExtent(50.f, 50.f, 50.f)
{
	PrimaryActorTick.bCanEverTick = false;

	InteractionCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionCollision"));
	SetRootComponent(InteractionCollision);
	InteractionCollision->SetBoxExtent(InteractionBoxExtent);

	// Interactive 채널에만 Block, 나머지는 Ignore
	InteractionCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	InteractionCollision->SetCanEverAffectNavigation(false);
}

void ATGInteractiveActor::BeginPlay()
{
	Super::BeginPlay();

	//	블루프린트 CDO가 InteractionCollision의 콜리전 프리셋을 덮어쓸 수 있으므로
	//	bInteractionEnabled 상태를 BeginPlay에서 강제 적용
	InteractionCollision->SetCollisionResponseToChannel(
		ECC_GameTraceChannel1,
		bInteractionEnabled ? ECR_Block : ECR_Ignore
	);

	//	메시 컴포넌트가 ECC_GameTraceChannel1을 Block하면 SetInteractionEnabled 로직을 방해하므로 강제로 Ignore
	TArray<UStaticMeshComponent*> MeshComps;
	GetComponents<UStaticMeshComponent>(MeshComps);
	for (UStaticMeshComponent* Mesh : MeshComps)
	{
		if (IsValid(Mesh))
		{
			Mesh->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
		}
	}
}

void ATGInteractiveActor::OnFocused_Implementation(ATGPlayer* Player) {}
void ATGInteractiveActor::OnUnfocused_Implementation(ATGPlayer* Player) {}
void ATGInteractiveActor::OnInteract_Implementation(ATGPlayer* Player) {}

void ATGInteractiveActor::SetInteractionEnabled(bool bEnabled)
{
	bInteractionEnabled = bEnabled;
	InteractionCollision->SetCollisionResponseToChannel(
		ECC_GameTraceChannel1,
		bEnabled ? ECR_Block : ECR_Ignore
	);
}
