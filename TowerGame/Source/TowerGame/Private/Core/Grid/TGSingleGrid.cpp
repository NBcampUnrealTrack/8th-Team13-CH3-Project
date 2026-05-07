// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Grid/TGSingleGrid.h"
#include "BaseTower/TGBaseTower.h"
#include "Components/BoxComponent.h"
#include "Core/Grid/TGGridBase.h"

ATGSingleGrid::ATGSingleGrid()
{
	PrimaryActorTick.bCanEverTick = false;

	//	ToDo : 헤더를 확인하고 삭제해야 할 때. 함께 삭제해주세요
	Visualizer = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	Visualizer->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube"));
	if (CubeMesh.Succeeded())
	{
		Visualizer->SetStaticMesh(CubeMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> CubeMaterial(TEXT("/Game/Core/Grid/Materials/M_GridTempMat.M_GridTempMat"));
	if (CubeMaterial.Succeeded())
	{
		Visualizer->SetMaterial(0, CubeMaterial.Object);
	}
	Visualizer->SetCustomDepthStencilValue(1);
	//	ToDoEnd
}

void ATGSingleGrid::SetParent(TObjectPtr<ATGGridBase> Parent)
{
	GridBase = Parent;
	MyPoint = Parent->GetPointFromPosition(GetActorLocation());
}

void ATGSingleGrid::SetBoxSize(float Size) const
{
	const float Scale = Size / 100.0f;
	const FVector CurrentScale = Visualizer->GetRelativeScale3D();
	Visualizer->SetRelativeScale3D(FVector(Scale, Scale, CurrentScale.Z));

	const float HalfSize = Size / 2.f;
	InteractionCollision->SetBoxExtent(FVector(HalfSize, HalfSize, InteractionCollision->GetUnscaledBoxExtent().Z));
}

void ATGSingleGrid::OnFocused_Implementation(ATGPlayer* Player)
{
	if (bIsPlacedTower)	return;
	Super::OnFocused_Implementation(Player);
	Visualizer->SetRenderCustomDepth(true);

	PlacedTower->SetPreviewMode();
}

void ATGSingleGrid::OnUnfocused_Implementation(ATGPlayer* Player)
{
	if (bIsPlacedTower)	return;
	Super::OnUnfocused_Implementation(Player);
	Visualizer->SetRenderCustomDepth(false);

	PlacedTower->Disable();
}

// void ATGSingleGrid::PlaceTower(TObjectPtr<ABaseTower> Tower)
// {
// 	PlacedTower = Tower;
// }
//
// void ATGSingleGrid::RemoveTower(TObjectPtr<ABaseTower> Tower)
// {
// 	PlacedTower = nullptr;
// }

void ATGSingleGrid::OnInteract_Implementation(ATGPlayer* Player)
{
	Super::OnInteract_Implementation(Player);

	if (!GridBase->CanBePlacedBuilding(MyPoint))	return;

	PlacedTower->FinalizeInstallation();
	bIsPlacedTower = true;
	SetInteractionEnabled(false);

	GridBase->PlacingBuilding(MyPoint);
}

void ATGSingleGrid::BeginPlay()
{
	Super::BeginPlay();

	PlacedTower = GetWorld()->SpawnActor<ABaseTower>();
	if (!IsValid(PlacedTower))	return;
	PlacedTower->SetActorLocation(GetActorLocation());
	PlacedTower->Disable();
}
