#include "UI/TGMiniMapWidget.h"
#include "Core/Grid/TGGridBase.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"

void UTGMiniMapWidget::SetGridBase(ATGGridBase* InGridBase)
{
	CachedGridBase = InGridBase;

	if (!IsValid(CachedGridBase))
	{
		UE_LOG(LogTemp, Warning, TEXT("MiniMapWidget: GridBase is invalid"));
		return;
	}

	BuildDebugMinimap();
}

void UTGMiniMapWidget::BuildDebugMinimap()
{
	if (!TileLayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("MiniMapWidget: TileLayer is null"));
		return;
	}

	if (!MarkerLayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("MiniMapWidget: MarkerLayer is null"));
		return;
	}

	if (!IsValid(CachedGridBase))
	{
		UE_LOG(LogTemp, Warning, TEXT("MiniMapWidget: CachedGridBase is invalid"));
		return;
	}

	const int32 GridX = CachedGridBase->GetGridX();
	const int32 GridY = CachedGridBase->GetGridY();

	if (GridX <= 0 || GridY <= 0)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("MiniMapWidget: Invalid Grid Size. GridX: %d, GridY: %d"),
			GridX,
			GridY
		);
		return;
	}

	TileLayer->ClearChildren();
	MarkerLayer->ClearChildren();

	const float TileWidth = MinimapSize.X / GridX;
	const float TileHeight = MinimapSize.Y / GridY;
	TileWidgetMap.Empty();

	for (int32 Y = 0; Y < GridY; ++Y)
	{
		for (int32 X = 0; X < GridX; ++X)
		{
			UBorder* TileWidget = WidgetTree->ConstructWidget<UBorder>(
				UBorder::StaticClass()
			);

			if (!TileWidget)
			{
				continue;
			}

			if ((X + Y) % 2 == 0)
			{
				TileWidget->SetBrushColor(
					FLinearColor(0.15f, 0.15f, 0.15f, 1.0f)
				);
			}
			else
			{
				TileWidget->SetBrushColor(
					FLinearColor(0.25f, 0.25f, 0.25f, 1.0f)
				);
			}

			const FIntPoint GridPoint(X, Y);
			TileWidgetMap.Add(GridPoint, TileWidget);

			UCanvasPanelSlot* CanvasSlot = TileLayer->AddChildToCanvas(TileWidget);

			if (CanvasSlot)
			{
				CanvasSlot->SetPosition(
					FVector2D(X * TileWidth, Y * TileHeight)
				);

				CanvasSlot->SetSize(
					FVector2D(TileWidth, TileHeight)
				);
			}
		}
	}

	AddGridPointMarker(
		0,
		0,
		FLinearColor(1.0f, 0.0f, 0.0f, 1.0f),
		TEXT("EntryPoint"),
		TileWidth,
		TileHeight
	);

	AddGridPointMarker(
		GridX - 1,
		GridY - 1,
		FLinearColor(0.0f, 1.0f, 0.0f, 1.0f),
		TEXT("ExitPoint"),
		TileWidth,
		TileHeight
	);
}

void UTGMiniMapWidget::AddGridPointMarker(
	int32 InGridX,
	int32 InGridY,
	const FLinearColor& MarkerColor,
	const FString& MarkerName,
	float TileWidth,
	float TileHeight
)
{
	if (!MarkerLayer)
	{
		return;
	}

	UTextBlock* MarkerText = WidgetTree->ConstructWidget<UTextBlock>(
		UTextBlock::StaticClass()
	);

	if (!MarkerText)
	{
		return;
	}

	const float MarkerSize = FMath::Min(TileWidth, TileHeight) * 0.8f;

	MarkerText->SetText(FText::FromString(TEXT("●")));
	MarkerText->SetColorAndOpacity(FSlateColor(MarkerColor));
	MarkerText->SetJustification(ETextJustify::Center);
	MarkerText->SetVisibility(ESlateVisibility::HitTestInvisible);

	FSlateFontInfo MarkerFont = MarkerText->GetFont();
	MarkerFont.Size = FMath::Max(8, FMath::RoundToInt(MarkerSize));
	MarkerText->SetFont(MarkerFont);

	UCanvasPanelSlot* MarkerSlot = MarkerLayer->AddChildToCanvas(MarkerText);

	if (MarkerSlot)
	{
		const float CenterX = (InGridX + 0.5f) * TileWidth;
		const float CenterY = (InGridY + 0.5f) * TileHeight;

		MarkerSlot->SetPosition(FVector2D(CenterX, CenterY));
		MarkerSlot->SetSize(FVector2D(MarkerSize, MarkerSize));

		MarkerSlot->SetAlignment(FVector2D(0.5f, 0.5f));
	}

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("MiniMap Marker Added: %s (%d, %d)"),
		*MarkerName,
		InGridX,
		InGridY
	);
}

void UTGMiniMapWidget::SetPlayerActor(AActor* InPlayerActor)
{
	CachedPlayerActor = InPlayerActor;

	if (!IsValid(CachedPlayerActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("MiniMapWidget: PlayerActor is invalid"));
		return;
	}

	CreatePlayerMarker();

	UpdatePlayerMarkerPosition();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			PlayerMarkerUpdateTimerHandle,
			this,
			&UTGMiniMapWidget::UpdatePlayerMarkerPosition,
			0.1f,
			true
		);
	}
}

void UTGMiniMapWidget::RegisterMonsterActor(AActor* InMonsterActor)
{
	if (!IsValid(InMonsterActor))
	{
		return;
	}

	if (!MarkerLayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("MiniMapWidget: MarkerLayer is null"));
		return;
	}

	if (MonsterMarkerMap.Contains(InMonsterActor))
	{
		return;
	}

	UTextBlock* MonsterMarker = WidgetTree->ConstructWidget<UTextBlock>(
		UTextBlock::StaticClass()
	);

	if (!MonsterMarker)
	{
		return;
	}

	MonsterMarker->SetText(FText::FromString(TEXT("●")));
	MonsterMarker->SetColorAndOpacity(
		FSlateColor(FLinearColor(1.0f, 0.15f, 0.0f, 1.0f))
	);
	MonsterMarker->SetJustification(ETextJustify::Center);
	MonsterMarker->SetVisibility(ESlateVisibility::HitTestInvisible);

	FSlateFontInfo MonsterFont = MonsterMarker->GetFont();
	MonsterFont.Size = 12;
	MonsterMarker->SetFont(MonsterFont);

	UCanvasPanelSlot* MonsterSlot = MarkerLayer->AddChildToCanvas(MonsterMarker);

	if (MonsterSlot)
	{
		MonsterSlot->SetSize(FVector2D(14.0f, 14.0f));
		MonsterSlot->SetAlignment(FVector2D(0.5f, 0.5f));
	}

	MonsterMarkerMap.Add(InMonsterActor, MonsterMarker);

	UpdateMonsterMarkers();
	StartMonsterMarkerUpdateTimer();
}

void UTGMiniMapWidget::UnregisterMonsterActor(AActor* InMonsterActor)
{
	if (!InMonsterActor)
	{
		return;
	}

	UTextBlock** FoundMarker = MonsterMarkerMap.Find(InMonsterActor);

	if (!FoundMarker)
	{
		return;
	}

	if (*FoundMarker)
	{
		(*FoundMarker)->RemoveFromParent();
	}

	MonsterMarkerMap.Remove(InMonsterActor);
}

void UTGMiniMapWidget::MarkWallAtGrid(const FIntPoint& GridPoint)
{
	UBorder** FoundTile = TileWidgetMap.Find(GridPoint);

	if (!FoundTile || !(*FoundTile))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("MiniMapWidget: Tile not found. X: %d, Y: %d"),
			GridPoint.X,
			GridPoint.Y
		);
		return;
	}

	// 벽 설치된 칸 표시 색상
	(*FoundTile)->SetBrushColor(
		FLinearColor(0.55f, 0.55f, 0.55f, 1.0f)
	);
}

void UTGMiniMapWidget::MarkTowerAtGrid(const FIntPoint& GridPoint, ETGTurretType TurretType)
{
	if (!MarkerLayer)
	{
		return;
	}

	if (!IsValid(CachedGridBase))
	{
		return;
	}

	const FLinearColor MarkerColor = GetTowerMarkerColor(TurretType);

	if (UTextBlock** ExistingMarker = TowerMarkerMap.Find(GridPoint))
	{
		if (*ExistingMarker)
		{
			(*ExistingMarker)->SetColorAndOpacity(FSlateColor(MarkerColor));
		}

		return;
	}

	const int32 GridX = CachedGridBase->GetGridX();
	const int32 GridY = CachedGridBase->GetGridY();

	if (GridX <= 0 || GridY <= 0)
	{
		return;
	}

	const float TileWidth = MinimapSize.X / GridX;
	const float TileHeight = MinimapSize.Y / GridY;
	const float MarkerSize = FMath::Min(TileWidth, TileHeight) * 1.f;

	UTextBlock* TowerMarker = WidgetTree->ConstructWidget<UTextBlock>(
		UTextBlock::StaticClass()
	);

	if (!TowerMarker)
	{
		return;
	}

	TowerMarker->SetText(FText::FromString(TEXT("◆")));
	TowerMarker->SetColorAndOpacity(FSlateColor(MarkerColor));
	TowerMarker->SetJustification(ETextJustify::Center);
	TowerMarker->SetVisibility(ESlateVisibility::HitTestInvisible);

	FSlateFontInfo TowerFont = TowerMarker->GetFont();
	TowerFont.Size = FMath::Max(8, FMath::RoundToInt(MarkerSize));
	TowerMarker->SetFont(TowerFont);

	UCanvasPanelSlot* TowerSlot = MarkerLayer->AddChildToCanvas(TowerMarker);

	if (TowerSlot)
	{
		const float CenterX = (GridPoint.X + 0.5f) * TileWidth;
		const float CenterY = (GridPoint.Y + 0.5f) * TileHeight;

		TowerSlot->SetPosition(FVector2D(CenterX, CenterY));
		TowerSlot->SetSize(FVector2D(MarkerSize, MarkerSize));
		TowerSlot->SetAlignment(FVector2D(0.5f, 0.5f));
	}

	TowerMarkerMap.Add(GridPoint, TowerMarker);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("MiniMapWidget: Tower marked. X: %d, Y: %d, Type: %d"),
		GridPoint.X,
		GridPoint.Y,
		static_cast<int32>(TurretType)
	);
}

void UTGMiniMapWidget::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PlayerMarkerUpdateTimerHandle);
		World->GetTimerManager().ClearTimer(MonsterMarkerUpdateTimerHandle);
	}

	Super::NativeDestruct();
}

void UTGMiniMapWidget::CreatePlayerMarker()
{
	if (!MarkerLayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("MiniMapWidget: MarkerLayer is null"));
		return;
	}

	if (PlayerMarker)
	{
		return;
	}

	PlayerMarker = WidgetTree->ConstructWidget<UTextBlock>(
		UTextBlock::StaticClass()
	);

	if (!PlayerMarker)
	{
		return;
	}

	PlayerMarker->SetText(FText::FromString(TEXT("▲")));
	PlayerMarker->SetColorAndOpacity(
		FSlateColor(FLinearColor(0.0f, 0.7f, 1.0f, 1.0f))
	);
	PlayerMarker->SetJustification(ETextJustify::Center);
	PlayerMarker->SetVisibility(ESlateVisibility::HitTestInvisible);

	FSlateFontInfo PlayerFont = PlayerMarker->GetFont();
	PlayerFont.Size = 25;
	PlayerMarker->SetFont(PlayerFont);

	UCanvasPanelSlot* PlayerSlot = MarkerLayer->AddChildToCanvas(PlayerMarker);

	if (PlayerSlot)
	{
		PlayerSlot->SetSize(FVector2D(20.0f, 20.0f));

		// 위치 기준점을 마커 중앙으로 설정
		PlayerSlot->SetAlignment(FVector2D(0.5f, 0.5f));
	}
}

void UTGMiniMapWidget::UpdatePlayerMarkerPosition()
{
	if (!IsValid(CachedPlayerActor))
	{
		return;
	}

	if (!PlayerMarker)
	{
		return;
	}

	const FVector PlayerWorldLocation = CachedPlayerActor->GetActorLocation();
	const FVector2D MinimapPosition = WorldLocationToMinimapPosition(PlayerWorldLocation);

	UCanvasPanelSlot* PlayerSlot = Cast<UCanvasPanelSlot>(PlayerMarker->Slot);

	if (PlayerSlot)
	{
		PlayerSlot->SetPosition(MinimapPosition);
	}
}

void UTGMiniMapWidget::UpdateMonsterMarkers()
{
	for (auto It = MonsterMarkerMap.CreateIterator(); It; ++It)
	{
		AActor* MonsterActor = It.Key();
		UTextBlock* MonsterMarker = It.Value();

		if (!IsValid(MonsterActor) || !IsValid(MonsterMarker))
		{
			if (IsValid(MonsterMarker))
			{
				MonsterMarker->RemoveFromParent();
			}

			It.RemoveCurrent();
			continue;
		}

		const FVector MonsterWorldLocation = MonsterActor->GetActorLocation();
		const FVector2D MinimapPosition = WorldLocationToMinimapPosition(MonsterWorldLocation);

		UCanvasPanelSlot* MonsterSlot = Cast<UCanvasPanelSlot>(MonsterMarker->Slot);

		if (MonsterSlot)
		{
			MonsterSlot->SetPosition(MinimapPosition);
		}
	}
}

void UTGMiniMapWidget::StartMonsterMarkerUpdateTimer()
{
	if (UWorld* World = GetWorld())
	{
		FTimerManager& TimerManager = World->GetTimerManager();

		if (!TimerManager.IsTimerActive(MonsterMarkerUpdateTimerHandle))
		{
			TimerManager.SetTimer(
				MonsterMarkerUpdateTimerHandle,
				this,
				&UTGMiniMapWidget::UpdateMonsterMarkers,
				0.1f,
				true
			);
		}
	}
}

FVector2D UTGMiniMapWidget::WorldLocationToMinimapPosition(const FVector& WorldLocation) const
{
	if (!IsValid(CachedGridBase))
	{
		return FVector2D::ZeroVector;
	}

	const int32 GridX = CachedGridBase->GetGridX();
	const int32 GridY = CachedGridBase->GetGridY();
	const float GridSize = CachedGridBase->GetGridSize();

	if (GridX <= 0 || GridY <= 0 || GridSize <= 0.0f)
	{
		return FVector2D::ZeroVector;
	}

	const FVector GridOrigin = CachedGridBase->GetActorLocation();

	const float MapWorldWidth = GridX * GridSize;
	const float MapWorldHeight = GridY * GridSize;

	const float LocalX = WorldLocation.X - GridOrigin.X;
	const float LocalY = WorldLocation.Y - GridOrigin.Y;

	const float NormalizedX = FMath::Clamp(LocalX / MapWorldWidth, 0.0f, 1.0f);
	const float NormalizedY = FMath::Clamp(LocalY / MapWorldHeight, 0.0f, 1.0f);

	return FVector2D(
		NormalizedX * MinimapSize.X,
		NormalizedY * MinimapSize.Y
	);
}

FLinearColor UTGMiniMapWidget::GetTowerMarkerColor(ETGTurretType TurretType) const
{
	switch (TurretType)
	{
	case ETGTurretType::None:
		return FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);

		// 실제 enum 이름에 맞게 수정하세요.
	case ETGTurretType::WeaponTower:
		return FLinearColor(0.1f, 0.4f, 1.0f, 1.0f);

	case ETGTurretType::DebuffTower:
		return FLinearColor(0.7f, 0.1f, 1.0f, 1.0f);

	case ETGTurretType::BuffTower:
		return FLinearColor(0.1f, 1.0f, 0.3f, 1.0f);

	default:
		return FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	}
}
