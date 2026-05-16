#include "UI/TGMiniMapWidget.h"
#include "Core/Grid/TGGridBase.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Border.h"
#include "Blueprint/WidgetTree.h"

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

	if (!IsValid(CachedGridBase))
	{
		UE_LOG(LogTemp, Warning, TEXT("MiniMapWidget: CachedGridBase is invalid"));
		return;
	}

	const int32 GridX = CachedGridBase->GetGridX();
	const int32 GridY = CachedGridBase->GetGridY();

	if (GridX <= 0 || GridY <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("MiniMapWidget: Invalid Grid Size. GridX: %d, GridY: %d"), GridX, GridY);
		return;
	}

	TileLayer->ClearChildren();

	const float TileWidth = MinimapSize.X / GridX;
	const float TileHeight = MinimapSize.Y / GridY;

	for (int32 Y = 0; Y < GridY; ++Y)
	{
		for (int32 X = 0; X < GridX; ++X)
		{
			UBorder* TileWidget = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());

			if (!TileWidget)
			{
				continue;
			}

			if ((X + Y) % 2 == 0)
			{
				TileWidget->SetBrushColor(FLinearColor(0.15f, 0.15f, 0.15f, 1.0f));
			}
			else
			{
				TileWidget->SetBrushColor(FLinearColor(0.25f, 0.25f, 0.25f, 1.0f));
			}

			UCanvasPanelSlot* CanvasSlot = TileLayer->AddChildToCanvas(TileWidget);

			if (CanvasSlot)
			{
				CanvasSlot->SetPosition(FVector2D(X * TileWidth, Y * TileHeight));
				CanvasSlot->SetSize(FVector2D(TileWidth, TileHeight));
			}
		}
	}
}
