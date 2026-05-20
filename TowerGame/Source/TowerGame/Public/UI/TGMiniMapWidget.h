#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TGMiniMapWidget.generated.h"

class UCanvasPanel;
class UTextBlock;
class ATGGridBase;
class AActor;

UCLASS()
class TOWERGAME_API UTGMiniMapWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetGridBase(ATGGridBase* InGridBase);

	// 플레이어 액터 등록
	void SetPlayerActor(AActor* InPlayerActor);

protected:
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* TileLayer;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* MarkerLayer;

private:
	UPROPERTY()
	ATGGridBase* CachedGridBase = nullptr;

	UPROPERTY()
	AActor* CachedPlayerActor = nullptr;

	UPROPERTY()
	UTextBlock* PlayerMarker = nullptr;

	FTimerHandle PlayerMarkerUpdateTimerHandle;

	UPROPERTY(EditAnywhere, Category = "Minimap")
	FVector2D MinimapSize = FVector2D(280.0f, 280.0f);

private:
	void BuildDebugMinimap();

	void AddGridPointMarker(
		int32 InGridX,
		int32 InGridY,
		const FLinearColor& MarkerColor,
		const FString& MarkerName,
		float TileWidth,
		float TileHeight
	);

	void CreatePlayerMarker();
	void UpdatePlayerMarkerPosition();

	FVector2D WorldLocationToMinimapPosition(const FVector& WorldLocation) const;
};
