#pragma once

#include "CoreMinimal.h"
#include "Enemies/TGAttackEnemyBase.h"
#include "TGBossBase.generated.h"

UENUM(BlueprintType)
enum class EBossPhase : uint8
{
	Phase1 UMETA(DisplayName = "Phase 1"),
	Phase2 UMETA(DisplayName = "Phase 2"),
	Phase3 UMETA(DisplayName = "Phase 3"),
};

UCLASS()
class TOWERGAME_API ATGBossBase : public ATGAttackEnemyBase
{
	GENERATED_BODY()

public:
	ATGBossBase();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual float TakeDamage(
		float DamageAmount,
		const FDamageEvent& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser
	) override;

	UFUNCTION(BlueprintPure, Category = "TowerGame|Boss")
	EBossPhase GetCurrentPhase() const { return CurrentPhase; }

	//	Phase 강제 전환
	UFUNCTION(BlueprintCallable, Category = "TowerGame|Boss")
	void ForcePhaseChange(EBossPhase Phase);

protected:
	// Phase2 진입 HP 비율 (0~1)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TowerGame|Boss")
	float Phase2HPRatio = 0.6f;

	// Phase3 진입 HP 비율 (0~1)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TowerGame|Boss")
	float Phase3HPRatio = 0.3f;

	// 패턴 실행 주기 (초)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TowerGame|Boss")
	float PatternInterval = 5.0f;

	// 페이즈 전환 시 호출
	virtual void OnPhaseChanged(EBossPhase NewPhase);

	// 현재 페이즈 패턴 실행
	virtual void ExecuteCurrentPattern();

private:
	EBossPhase CurrentPhase = EBossPhase::Phase1;
	float MaxHP = 0.f;

	FTimerHandle PatternTimerHandle;

	void CheckPhaseTransition();
	void StartPattern();
	void StopPattern();
};
