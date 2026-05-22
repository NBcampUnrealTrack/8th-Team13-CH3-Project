// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TGPlayerWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Player/TGPlayer.h"
#include "Enemies/TGNavigationManager.h"
#include "Enemies/TGCoreBase.h"
#include "Kismet/GameplayStatics.h"
#include "Core/GameFlow/TGGameMode.h"
#include "Core/Grid/TGGridBase.h"
#include "Enemies/TGBossBase.h"
#include "UI/TGMiniMapWidget.h"
#include "Enemies/TGEnemyBase.h"
#include "Enemies/TGWaveManager.h"

void UTGPlayerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	player = Cast<ATGPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
	// Focus Enemy 변경 이벤트 구독
	if (player){
		player->OnFocusedEnemyChanged.AddUniqueDynamic(this, &UTGPlayerWidget::HandleFocusedEnemyChanged);
		player->OnPlayerHpChanged.AddUniqueDynamic(this, &UTGPlayerWidget::UpdatePlayerHPBar);
		player->OnEvadeChanged.AddUniqueDynamic(this, &UTGPlayerWidget::UpdateEvadeBar);
		player->OnWeaponChanged.AddDynamic(this, &UTGPlayerWidget::UpdateWeaponImage);

		// 회피 게이지 생성
		for (int i = 0; i < player->GetMaxEvadeCount(); i++)
		{
			UProgressBar* EvadeItem = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass());
			FProgressBarStyle BarStyle;
			BarStyle.SetBackgroundImage(FSlateBrush());
			BarStyle.SetFillImage(FSlateBrush());
			EvadeItem->SetFillColorAndOpacity(FLinearColor(0.f, 0.5f, 0.5f, 1.0f));
			EvadeItem->SetWidgetStyle(BarStyle);
			UHorizontalBoxSlot* EvadeSlot = Evade_BarGroup->AddChildToHorizontalBox(EvadeItem);
			if (EvadeSlot)
			{
				FSlateChildSize ChildSize;
				ChildSize.Value = 1.0f;
				EvadeSlot->SetSize(ChildSize);
				EvadeSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
				EvadeSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
				EvadeSlot->SetPadding(FMargin(4.0f, 0.0f));
			}
			Evade_Bars.Add(EvadeItem);
		}
		UpdateEvadeBar(player->GetCurrentEvadeCount(), player->GetCurrentEvadeCooldown() / player->GetMaxEvadeCooldown());
	}

	// Wave 시작 이벤트, Boss Spawn 이벤트 구독
	WaveManager = ATGWaveManager::Get(this);
	if (WaveManager){
		WaveManager->OnWaveStarted.AddUniqueDynamic(this, &UTGPlayerWidget::HandleWaveStarted);
		WaveManager->OnBossSpawned.AddDynamic(this, &UTGPlayerWidget::BindBoss);
	}

	NavigationManager = ATGNavigationManager::Get(this);
	if (NavigationManager){
		// Core 변경 이벤트를 구독하고, 수동 초기화 1회 진행
		NavigationManager->OnCurrentCoreChanged.AddDynamic(this, &UTGPlayerWidget::UpdateCurrentCore);
		UpdateCurrentCore(NavigationManager->GetCurrentCoreActor());
	}

	// 자원 변경 이벤트를 구독하고, 자원 수동 초기화 1회 진행
	GameMode = Cast<ATGGameMode>(UGameplayStatics::GetGameMode(this));
	if(GameMode){
		GameMode->OnEnergyChanged.AddUniqueDynamic(this, &UTGPlayerWidget::HandleEnergyChanged);

		HandleEnergyChanged(GameMode->GetCurrentEnergy());
	}
}

void UTGPlayerWidget::NativeDestruct()
{
	// 타이머 해제
	if (UWorld* World = GetWorld()){
		World->GetTimerManager().ClearTimer(FocusedEnemyHideTimerHandle);
	}

	for (UProgressBar* b : Evade_Bars)
	{
		b = nullptr;
	}
	Evade_Bars.Empty();
	Evade_BarGroup->ClearChildren();

	// 구독했던 델리게이트들 해제
	if (player){
		player->OnFocusedEnemyChanged.RemoveDynamic(this, &UTGPlayerWidget::HandleFocusedEnemyChanged);
		player->OnPlayerHpChanged.RemoveDynamic(this, &UTGPlayerWidget::UpdatePlayerHPBar);
	}

	if (WaveManager){
		WaveManager->OnWaveStarted.RemoveDynamic(this, &UTGPlayerWidget::HandleWaveStarted);
		WaveManager->OnBossSpawned.RemoveDynamic(this, &UTGPlayerWidget::BindBoss);
		UnbindBoss();
	}

	if (NavigationManager){
		NavigationManager->OnCurrentCoreChanged.RemoveDynamic(this, &UTGPlayerWidget::UpdateCurrentCore);
	}

	if (core){
		core->OnCoreHPChanged.RemoveDynamic(this, &UTGPlayerWidget::HandleCoreHPChanged);
		core = nullptr;
	}

	if (GameMode){
		GameMode->OnEnergyChanged.RemoveDynamic(this, &UTGPlayerWidget::HandleEnergyChanged);
	}

	if (GridBase)
	{
		GridBase->OnGridBuildingPlaced.RemoveDynamic(this, &UTGPlayerWidget::HandleGridBuildingPlaced);

		GridBase->OnGridTowerPlaced.RemoveDynamic(this, &UTGPlayerWidget::HandleGridTowerPlaced);
	}

	UnbindFocusedEnemy();
	Super::NativeDestruct();
}

void UTGPlayerWidget::HandleWaveStarted(int32 WaveIndex)
{
	if (!Txt_CurrentWave) return;

	// Text 설정 및 애니메이션 실행
	Txt_CurrentWave->SetText(FText::FromString(FString::Printf(TEXT("Wave : %d"), WaveIndex+1)));
	if (Anim_WaveOpacity) PlayAnimation(Anim_WaveOpacity);
}

void UTGPlayerWidget::UpdateBossHPBar(float CurrentHP, float MaxHP)
{
	if (!PB_BossHP) return;

	// Phase 별 UI 최대 최소 비율 변경
	const float PhaseMaxHP = Boss->GetCurrentPhaseMaxHP();
	const float PhaseMinHP = Boss->GetCurrentPhaseMinHP();
	const float PhaseHPRange = PhaseMaxHP - PhaseMinHP;

	const float Percent = FMath::Clamp((CurrentHP - PhaseMinHP) / PhaseHPRange, 0.0f, 1.0f);
	PB_BossHP->SetPercent(Percent);
}

void UTGPlayerWidget::HandleBossRemoved(ATGBossBase* RemovedBoss)
{
	if (RemovedBoss != Boss) return;

	UnbindBoss();

	if (!PB_BossHP) return;
	PB_BossHP->SetVisibility(ESlateVisibility::Hidden);
}

void UTGPlayerWidget::UpdateCurrentCore(ATGCoreBase* NewCore)
{
	// NewCore가 변하지 않은 상태에서의 중복 호출 방어 코드
	if (core == NewCore){
		if (core){
			HandleCoreHPChanged(core->GetCurrentHP(), core->GetMaxHP());
		}
		return;
	}

	// 기존 델리게이트 구독 해제
	if (core){
		core->OnCoreHPChanged.RemoveDynamic(this, &UTGPlayerWidget::HandleCoreHPChanged);
	}

	// Core HP 변경 이벤트 구독 및 수동 초기화 1회 진행
	core = NewCore;
	if (core){
		core->OnCoreHPChanged.AddUniqueDynamic(this, &UTGPlayerWidget::HandleCoreHPChanged);

		HandleCoreHPChanged(core->GetCurrentHP(), core->GetMaxHP());
	}else if (CoreHP_Bar){
		CoreHP_Bar->SetPercent(0);
	}
}

void UTGPlayerWidget::HandleCoreHPChanged(float CurrentHP, float MaxHP)
{
	if (!CoreHP_Bar) return;

	const float Percent = CurrentHP / MaxHP;
	CoreHP_Bar->SetPercent(FMath::Clamp(Percent, 0.0f, 1.0f));
}

void UTGPlayerWidget::HandleEnergyChanged(int32 NewEnergy)
{
	if (!EnergyText) return;

	EnergyText->SetText(FText::AsNumber(NewEnergy));

}

void UTGPlayerWidget::HandleFocusedEnemyChanged(ATGEnemyBase* NewEnemy)
{
	UWorld* World = GetWorld();
	if (!World) return;

	// 기존 UI 숨김 타이머 제거
	World->GetTimerManager().ClearTimer(FocusedEnemyHideTimerHandle);

	// 0.5초 뒤 UI 숨김
	if (!NewEnemy){
		World->GetTimerManager().SetTimer(
			FocusedEnemyHideTimerHandle,
			this,
			&UTGPlayerWidget::HideFocusedEnemyInfo,
			0.5f,
			false
		);
		return;
	}

	BindFocusedEnemy(NewEnemy);

	// EnemyType UI 반영
	if (Txt_EnemyType){
		Txt_EnemyType->SetText(FText::FromString(FocusedEnemy->GetEnemyType()));
		Txt_EnemyType->SetVisibility(ESlateVisibility::Visible);
	}

	// Enemy 체력 UI 반영
	if (PB_EnemyHP){
		UpdateFocusedEnemyHPBar(FocusedEnemy->GetCurrentHP(), FocusedEnemy->GetMaxHP());
		PB_EnemyHP->SetVisibility(ESlateVisibility::Visible);
	}

}

void UTGPlayerWidget::UpdateFocusedEnemyHPBar(float CurrentHP, float MaxHP)
{
	if (!PB_EnemyHP) return;

	const float EnemyRatio = FMath::Clamp(CurrentHP / MaxHP, 0.0f, 1.0f);
	PB_EnemyHP->SetPercent(EnemyRatio);
}

void UTGPlayerWidget::UpdatePlayerHPBar(float CurrentHP, float MaxHP)
{
	if (!player) return;
	HP_Bar->SetPercent(FMath::Clamp(CurrentHP / MaxHP, 0.0f, 1.0f));
}

void UTGPlayerWidget::UpdateEvadeBar(int32 CurrentEvadeCount, float CooldownRate)
{
	int idx;
	for (idx = 0; idx < Evade_Bars.Num(); idx++)
	{
		if (Evade_Bars.IsValidIndex(idx))
		{
			if (idx < CurrentEvadeCount)
				Evade_Bars[idx]->SetPercent(1.f);
			else if(idx == CurrentEvadeCount)
				Evade_Bars[idx]->SetPercent(CooldownRate);
			else
				Evade_Bars[idx]->SetPercent(0.f);
		}
	}
}

void UTGPlayerWidget::UpdateWeaponImage(UTGWeaponBase* Weapon)
{
	UMaterialInstanceDynamic* mat = WeaponImage->GetDynamicMaterial();
	if (Weapon && Weapon->GetAsset()->WeaponImage)
	{
		WeaponImage->SetVisibility(ESlateVisibility::Visible);
		mat->SetTextureParameterValue(TEXT("WeaponTex"), Weapon->GetAsset()->WeaponImage);
	}
	else
		WeaponImage->SetVisibility(ESlateVisibility::Hidden);
}

void UTGPlayerWidget::HandleFocusedEnemyRemoved(ATGEnemyBase* RemovedEnemy)
{
	if (RemovedEnemy != FocusedEnemy) return;

	if (UWorld* World = GetWorld()){
		World->GetTimerManager().ClearTimer(FocusedEnemyHideTimerHandle);
	}

	HideFocusedEnemyInfo();
}

void UTGPlayerWidget::HideFocusedEnemyInfo()
{
	UnbindFocusedEnemy();
	FocusedEnemy = nullptr;

	// FocusedEnemy가 없으면 Enemy이름과 HP 숨김처리
	if (Txt_EnemyType){
		Txt_EnemyType->SetVisibility(ESlateVisibility::Hidden);
	}

	if (PB_EnemyHP){
		PB_EnemyHP->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UTGPlayerWidget::HandleGridBuildingPlaced(FIntPoint GridPoint)
{
	if (!MiniMapWidget)
	{
		return;
	}

	MiniMapWidget->MarkWallAtGrid(GridPoint);
}

void UTGPlayerWidget::BindFocusedEnemy(ATGEnemyBase* NewEnemy)
{
	UnbindFocusedEnemy();

	FocusedEnemy = NewEnemy;
	if (!FocusedEnemy) return;

	// Enemy 체력 변동, 삭제 델리게이트 구독
	FocusedEnemy->OnEnemyHpChanged.AddUniqueDynamic(
		this, &UTGPlayerWidget::UpdateFocusedEnemyHPBar);
	FocusedEnemy->OnEnemyRemoved.AddUniqueDynamic(
		this, &UTGPlayerWidget::HandleFocusedEnemyRemoved);
}

void UTGPlayerWidget::UnbindFocusedEnemy()
{
	if (!FocusedEnemy) return;

	// Enemy 체력 변동, 삭제 델리게이트 구독 해제
	FocusedEnemy->OnEnemyHpChanged.RemoveDynamic(this, &UTGPlayerWidget::UpdateFocusedEnemyHPBar);
	FocusedEnemy->OnEnemyRemoved.RemoveDynamic(this, &UTGPlayerWidget::HandleFocusedEnemyRemoved);

	FocusedEnemy = nullptr;
}

void UTGPlayerWidget::BindBoss(ATGBossBase* NewBoss)
{
	UnbindBoss();

	Boss = NewBoss;
	if (!Boss) return;

	// 보스 HP, Boss 제거 이벤트 등록
	Boss->OnBossHpChanged.AddDynamic(this, &UTGPlayerWidget::UpdateBossHPBar);
	Boss->OnBossRemoved.AddDynamic(this, &UTGPlayerWidget::HandleBossRemoved);

	if (Txt_BossName){
		Txt_BossName->SetText(Boss->GetBossName());
		Txt_BossName->SetVisibility(ESlateVisibility::Visible);
	}

	UpdateBossHPBar(Boss->GetCurrentHP(), Boss->GetMaxHP());

	if (PB_BossHP){
		PB_BossHP->SetVisibility(ESlateVisibility::Visible);
	}
}

void UTGPlayerWidget::UnbindBoss()
{
	if (!Boss) return;

	// 보스 HP 변동 및 제거 이벤트 해제
	Boss->OnBossHpChanged.RemoveDynamic(this, &UTGPlayerWidget::UpdateBossHPBar);
	Boss->OnBossRemoved.RemoveDynamic(this, &UTGPlayerWidget::HandleBossRemoved);

	Boss = nullptr;
}

void UTGPlayerWidget::HandlePauseClicked()
{
	if (ATGGameMode* GM = Cast<ATGGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->PauseGameFlow();
	}
}

void UTGPlayerWidget::SetGridBase(ATGGridBase* InGridBase)
{
	if (!MiniMapWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerWidget: MiniMapWidget is null"));
		return;
	}

	if (!IsValid(InGridBase))
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerWidget: GridBase is invalid"));
		return;
	}

	GridBase = InGridBase;

	MiniMapWidget->SetGridBase(InGridBase);

	GridBase->OnGridBuildingPlaced.AddUniqueDynamic(
		this,
		&UTGPlayerWidget::HandleGridBuildingPlaced
	);

	GridBase->OnGridTowerPlaced.AddUniqueDynamic(
		this,
		&UTGPlayerWidget::HandleGridTowerPlaced
	);

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);

	if (PlayerPawn)
	{
		MiniMapWidget->SetPlayerActor(PlayerPawn);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerWidget: PlayerPawn is null"));
	}
}

void UTGPlayerWidget::RegisterMonsterToMiniMap(AActor* MonsterActor)
{
	if (!MiniMapWidget)
	{
		return;
	}

	MiniMapWidget->RegisterMonsterActor(MonsterActor);
}

void UTGPlayerWidget::UnregisterMonsterFromMiniMap(AActor* MonsterActor)
{
	if (!MiniMapWidget)
	{
		return;
	}

	MiniMapWidget->UnregisterMonsterActor(MonsterActor);
}

void UTGPlayerWidget::HandleGridTowerPlaced(FIntPoint GridPoint, ETGTurretType TurretType)
{
	if (!MiniMapWidget)
	{
		return;
	}

	MiniMapWidget->MarkTowerAtGrid(GridPoint, TurretType);
}
