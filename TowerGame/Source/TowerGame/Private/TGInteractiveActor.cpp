// Fill out your copyright notice in the Description page of Project Settings.

#include "TGInteractiveActor.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"

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

	GetWorldTimerManager().SetTimer(
		DebugDrawTimerHandle,
		this,
		&ATGInteractiveActor::DrawDebugCollisionBox,
		2.0f,
		false
	);
}

void ATGInteractiveActor::DrawDebugCollisionBox()
{
	const FVector Extent = InteractionCollision->GetScaledBoxExtent();
	DrawDebugBox(GetWorld(), GetActorLocation(), Extent, FColor::Yellow, true);
}

void ATGInteractiveActor::OnFocused_Implementation(ATGPlayer* Player) {}
void ATGInteractiveActor::OnUnfocused_Implementation(ATGPlayer* Player) {}
void ATGInteractiveActor::OnInteract_Implementation(ATGPlayer* Player) {}

void ATGInteractiveActor::SetInteractionEnabled(bool bEnabled)
{
	if (bInteractionEnabled == bEnabled) return;

	bInteractionEnabled = bEnabled;
	InteractionCollision->SetCollisionResponseToChannel(
		ECC_GameTraceChannel1,
		bEnabled ? ECR_Block : ECR_Ignore
	);
}
