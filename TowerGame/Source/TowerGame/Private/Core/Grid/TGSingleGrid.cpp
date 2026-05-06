// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Grid/TGSingleGrid.h"
#include "BaseTower/TGBaseTower.h"

ATGSingleGrid::ATGSingleGrid()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");

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
	//	ToDoEnd
}

void ATGSingleGrid::SetParent(TObjectPtr<ATGGridBase> Parent)
{
	GridBase = Parent;
}

void ATGSingleGrid::SetBoxSize(float Size) const
{
	const float Scale = Size / 100.0f;
	const FVector CurrentScale = Visualizer->GetRelativeScale3D();
	Visualizer->SetRelativeScale3D(FVector(Scale, Scale, CurrentScale.Z));
}

void ATGSingleGrid::OnFocused_Implementation(ATGPlayer* Player)
{
	Super::OnFocused_Implementation(Player);
	Visualizer->SetRenderCustomDepth(true);
	Visualizer->SetCustomDepthStencilValue(1);
}

void ATGSingleGrid::OnUnfocused_Implementation(ATGPlayer* Player)
{
	Super::OnUnfocused_Implementation(Player);
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

	//	TODO : PlacedTower->Enable();
}

void ATGSingleGrid::BeginPlay()
{
	Super::BeginPlay();

	PlacedTower = GetWorld()->SpawnActor<ABaseTower>();
	if (!IsValid(PlacedTower))	return;
	PlacedTower->SetActorLocation(GetActorLocation());
	PlacedTower->BaseMesh->SetVisibility(false);
	//	TODO : PlacedTower->Disable();
}
