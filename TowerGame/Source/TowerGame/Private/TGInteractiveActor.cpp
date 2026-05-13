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
	//	손자 컴포넌트는 확인하지 않음
	const TArray<USceneComponent*>& MeshComps = RootComponent->GetAttachChildren();;

	if (MeshComps.IsEmpty()) return;

	FBox CombinedBox(ForceInit);
	for (USceneComponent* Comp : MeshComps)
	{
		UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(Comp);
		if (!IsValid(Mesh))continue;
		if (!Mesh || !Mesh->GetStaticMesh()) continue;
		// 메쉬 로컬 바운드에 컴포넌트의 상대 트랜스폼(위치·회전·스케일)을 적용해 액터 로컬 공간으로 변환
		FBox MeshBox = Mesh->GetStaticMesh()->GetBoundingBox();
		CombinedBox += MeshBox.TransformBy(Mesh->GetRelativeTransform());
	}

	if (CombinedBox.IsValid)
	{
		InteractionCollision->SetBoxExtent(CombinedBox.GetExtent());
	}
}

void ATGInteractiveActor::DrawDebugCollisionBox()
{
	//	디버깅용 바운드 박스 그리기 함수입니다.
	//	사용 안함
	return;
	//const FVector Extent = InteractionCollision->GetScaledBoxExtent();
	//DrawDebugBox(GetWorld(), GetActorLocation(), Extent, FColor::Yellow, true);
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
