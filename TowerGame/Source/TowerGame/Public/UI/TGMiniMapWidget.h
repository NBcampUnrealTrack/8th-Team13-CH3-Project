#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TGMiniMapWidget.generated.h"

class UCanvasPanel;
class UTextBlock;
class ATGGridBase;

UCLASS()
class TOWERGAME_API UTGMiniMapWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetGridBase(ATGGridBase* InGridBase);

protected:
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* TileLayer;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* MarkerLayer;

	UPROPERTY()
	ATGGridBase* CachedGridBase = nullptr;

private:
	void BuildDebugMinimap();

	void AddGridPointMarker(
		int32 GridX,
		int32 GridY,
		const FLinearColor& MarkerColor,
		const FString& MarkerName,
		float TileWidth,
		float TileHeight
	);

	UPROPERTY(EditAnywhere, Category = "Minimap")
	FVector2D MinimapSize = FVector2D(280.0f, 280.0f);

	UPROPERTY(EditAnywhere, Category = "Minimap")
	FVector2D PointMarkerSize = FVector2D(24.0f, 24.0f);
};
