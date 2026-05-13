// Fill out your copyright notice in the Description page of Project Settings.

#include "TGInteractiveActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
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

	SyncCollisionToMeshBounds();

	GetWorldTimerManager().SetTimer(
		DebugDrawTimerHandle,
		this,
		&ATGInteractiveActor::DrawDebugCollisionBox,
		2.0f,
		false
	);
}

void ATGInteractiveActor::SyncCollisionToMeshBounds() const
{
	TArray<UStaticMeshComponent*> MeshComps;
	GetComponents<UStaticMeshComponent>(MeshComps);

	if (MeshComps.IsEmpty()) return;

	FBox LocalBox(ForceInit);
	float WorldMaxZ = GetActorLocation().Z;

	for (UStaticMeshComponent* Mesh : MeshComps)
	{
		if (!IsValid(Mesh) || !Mesh->GetStaticMesh()) continue;

		if (Mesh->GetAttachParent() == InteractionCollision)
		{	//	직속 자식은 XY까지 검사
			// XY: 액터 로컬 공간 기준
			LocalBox += Mesh->GetStaticMesh()->GetBoundingBox().TransformBy(Mesh->GetRelativeTransform());
		}
		//	손자 이하는 Z만 검사
		// Z: 월드 공간 기준 메쉬 최상단 높이
		const FBox MeshWorldBox = Mesh->GetStaticMesh()->GetBoundingBox().TransformBy(Mesh->GetComponentTransform());
		WorldMaxZ = FMath::Max(WorldMaxZ, MeshWorldBox.Max.Z);
	}

	if (LocalBox.IsValid)
	{
		FVector Extent = LocalBox.GetExtent();
		Extent.Z = WorldMaxZ - GetActorLocation().Z;
		InteractionCollision->SetBoxExtent(Extent);
	}
}

void ATGInteractiveActor::DrawDebugCollisionBox()
{
	//	디버깅용 바운드 박스 그리기 함수입니다.
	//	사용 안함

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
