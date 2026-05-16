#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TGMiniMapWidget.generated.h"

class UCanvasPanel;
class UBorder;
class ATGGridBase;

UCLASS()
class TOWERGAME_API UTGMiniMapWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetGridBase(ATGGridBase* InGridBase);
	//void BuildMinimap();

protected:
	//virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* TileLayer;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* MarkerLayer;

	UPROPERTY()
	ATGGridBase* CachedGridBase;
private:
	void BuildDebugMinimap();

	UPROPERTY(EditAnywhere, Category = "Minimap")
	FVector2D MinimapSize = FVector2D(280.0f, 280.0f);

	UPROPERTY(EditAnywhere, Category = "Minimap")
	int32 DebugGridWidth = 10;

	UPROPERTY(EditAnywhere, Category = "Minimap")
	int32 DebugGridHeight = 10;
};
