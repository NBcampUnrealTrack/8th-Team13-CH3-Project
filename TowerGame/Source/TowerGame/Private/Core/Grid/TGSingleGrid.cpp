// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Grid/TGSingleGrid.h"
#include "BaseTower/TGBaseTower.h"
#include "Components/BoxComponent.h"
#include "Core/GameFlow/TGGameMode.h"
#include "Core/Grid/TGGridBase.h"
#include "Kismet/GameplayStatics.h"

ATGSingleGrid::ATGSingleGrid()
{
	PrimaryActorTick.bCanEverTick = false;

	//	ToDo : 헤더를 확인하고 삭제해야 할 때. 함께 삭제해주세요
	Visualizer = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	SetRootComponent(Visualizer);
	InteractionCollision->SetupAttachment(Visualizer);

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

	//	외곽선 표현을 위한 커스텀 뎁스 스텐실 등록
	Visualizer->SetCustomDepthStencilValue(1);

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
}

void ATGSingleGrid::ResetGrid()
{
	if (PlacedTower)
	{
		PlacedTower->ResetTower();
	}
	bIsPlacedTower = false;
	Visualizer->SetRenderCustomDepth(false);
	SetInteractionEnabled(true);

	if (GridBase)
	{
		GridBase->RemoveBuilding(MyPoint);
	}
}

void ATGSingleGrid::OnFocused_Implementation(ATGPlayer* Player)
{
	if (bIsPlacedTower)	return;
	if (!PlacedTower)	return;
	Super::OnFocused_Implementation(Player);
	//	커스텀 뎁스 패스에 등록 -> 포스트 프로세싱 가능
	Visualizer->SetRenderCustomDepth(true);

	PlacedTower->SetPreviewMode();
}

void ATGSingleGrid::OnUnfocused_Implementation(ATGPlayer* Player)
{
	if (bIsPlacedTower)	return;
	if (!PlacedTower)	return;
	Super::OnUnfocused_Implementation(Player);
	//	커스텀 뎁스 패스에서 제외
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
	if (!PlacedTower)	return;

	ATGGameMode* GM = Cast<ATGGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GM || !GM->SpendEnergy(50)) return;

	if (!GridBase->CanBePlacedBuilding(MyPoint))
	{
		GM->AddEnergy(50);
		return;
	}

	PlacedTower->FinalizeInstallation();

	// 미니맵 이벤트
	PlacedTower->OnMountedTowerInstalled.AddUniqueDynamic(
		this,
		&ATGSingleGrid::HandleMountedTowerInstalled
	);

	bIsPlacedTower = true;
	SetInteractionEnabled(false);
	Visualizer->SetRenderCustomDepth(false);

	GridBase->PlacingBuilding(MyPoint);
}

void ATGSingleGrid::HandleMountedTowerInstalled(ETGTurretType TurretType)
{
	if (!GridBase)
	{
		return;
	}

	GridBase->PlacingTower(MyPoint, TurretType);
}

void ATGSingleGrid::BeginPlay()
{
	Super::BeginPlay();

	BaseTowerClass = GridBase->GetTowerClassOf();
	PlacedTower = GetWorld()->SpawnActor<ABaseTower>(BaseTowerClass);
	if (!IsValid(PlacedTower))	return;
	SetInteractionEnabled(true);
	PlacedTower->SetActorLocation(GetActorLocation());
	PlacedTower->Disable();
}
