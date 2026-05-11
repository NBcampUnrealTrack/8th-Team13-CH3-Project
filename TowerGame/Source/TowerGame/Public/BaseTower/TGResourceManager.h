#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TGResourceManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnResourceChanged, int32, CurrentResource);

UCLASS()
class TOWERGAME_API ATGResourceManager : public AActor
{
	GENERATED_BODY()

public:
	ATGResourceManager();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	//	싱글톤 인스턴스 반환
	static ATGResourceManager* Get(const UObject* WorldContextObject);

	//	리소스 추가 (몬스터 처치 시 호출)
	UFUNCTION(BlueprintCallable, Category = "TowerGame|Resource")
	void AddResource(int32 Amount);

	//	리소스 사용 (타워 업그레이드 시 호출)
	//	성공 시 true, 리소스 부족 시 false 반환
	UFUNCTION(BlueprintCallable, Category = "TowerGame|Resource")
	bool UseResource(int32 Amount);

	//	현재 리소스 반환
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TowerGame|Resource")
	int32 GetCurrentResource() const { return CurrentResource; }

	//	리소스 변경 이벤트 (UI 갱신용)
	UPROPERTY(BlueprintAssignable, Category = "TowerGame|Resource")
	FOnResourceChanged OnResourceChanged;

private:
	//	현재 보유 리소스
	UPROPERTY(VisibleAnywhere, Category = "TowerGame|Resource")
	int32 CurrentResource = 0;

	static TWeakObjectPtr<ATGResourceManager> Instance;
};
