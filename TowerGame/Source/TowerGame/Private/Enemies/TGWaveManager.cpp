// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/TGWaveManager.h"

#include "EngineUtils.h"
#include "Enemies/TGEnemyBase.h"
#include "Enemies/TGEnemySpawner.h"
#include "Enemies/TGWaveData.h"

TWeakObjectPtr<ATGWaveManager> ATGWaveManager::Instance;

// Sets default values
ATGWaveManager::ATGWaveManager():
	SpawnInterval(1.0f),
	DelayBeforeNextWave(5.0f),
	NextWaveIndex(0),
	ActiveWaveIndex(0),
	bIsSpawning(false),
	PendingSpawnIndex(0),
	NextSpawnerIndex(0)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void ATGWaveManager::BeginPlay()
{
	Super::BeginPlay();

	if (Instance.IsValid() && Instance.Get() != this && Instance->GetWorld() == GetWorld()){
		UE_LOG(LogTemp, Error, TEXT("TGWaveManager가 2개 이상 존재합니다."));
		return;
	}

	Instance = this;
}

void ATGWaveManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Instance.Get() == this){
		Instance.Reset();
	}

	Super::EndPlay(EndPlayReason);
}

ATGWaveManager* ATGWaveManager::Get(const UObject* WorldContextObject)
{
	if (!WorldContextObject) return nullptr;

	UWorld* World = WorldContextObject->GetWorld();
	if (!World) return nullptr;

	// 등록된 인스턴스가 있고 같은 World 소속이면 그대로 사용한디
	if (Instance.IsValid() && Instance->GetWorld() == World){
		return Instance.Get();
	}

	// 아직 등록된 인스턴스가 없거나 World 가 다르면 현재 World에서 탐색한다
	for (TActorIterator<ATGWaveManager> It(World); It; ++It){
		Instance = *It;
		return Instance.Get();
	}

	return nullptr;
}

void ATGWaveManager::AddEnemySpawner(ATGEnemySpawner* InEnemySpawner)
{
	if (!InEnemySpawner) return;

	EnemySpawners.AddUnique(InEnemySpawner);
}

void ATGWaveManager::RemoveEnemySpawner(ATGEnemySpawner* InEnemySpawner)
{
	if (!InEnemySpawner) return;

	// EnemySpawner 제거
	const int32 RemovedIndex = EnemySpawners.IndexOfByKey(InEnemySpawner);
	EnemySpawners.RemoveAt(RemovedIndex);

	// 남아있는 EnemySpawner가 없을 경우
	if (EnemySpawners.Num() == 0){
		NextWaveIndex = 0;
		return;
	}

	if (RemovedIndex != INDEX_NONE && NextSpawnerIndex >= EnemySpawners.Num()) NextSpawnerIndex = 0;
}

bool ATGWaveManager::StartNextWave()
{
	if (bIsSpawning) return false;

	if (GetWorldTimerManager().IsTimerActive(NextWaveDelayTimerHandle)){
		GetWorldTimerManager().ClearTimer(NextWaveDelayTimerHandle);
	}

	if (EnemySpawners.Num() == 0) return  false;
	if (!HasNextWave()) return  false;

	// 현재 Wave 스폰 상태 초기화
	InitializeSpawnState(NextWaveIndex);
	NextWaveIndex++;

	// 웨이브 시작 이벤튼
	OnWaveStarted.Broadcast(ActiveWaveIndex);

	// 유효한 SpawnGroup이 없으면 Spawn 완료 처리
	if (PendingSpawnQueue.Num() == 0){
		FinishCurrentWaveSpawn();
		return true;
	}

	bIsSpawning = true;

	// Spawn Timer 시작
	GetWorldTimerManager().SetTimer(
		SpawnTimerHandle,
		this,
		&ATGWaveManager::SpawnNextEnemy,
		SpawnInterval,
		true,
		0
	);

	return true;
}

void ATGWaveManager::SpawnNextEnemy()
{
	// 현재 Wave에서 더 이상 스폰할 Enemy가 없으면 종료
	if (!PendingSpawnQueue.IsValidIndex(PendingSpawnIndex)){
		FinishCurrentWaveSpawn();
		return;
	}

	if (EnemySpawners.Num() == 0){
		FinishCurrentWaveSpawn();
		return;
	}

	TSubclassOf<ATGEnemyBase> EnemyClass = PendingSpawnQueue[PendingSpawnIndex];

	// 유효하지 않은 EnemyClass 제외
	if (!EnemyClass){
		PendingSpawnIndex++;
		return;
	}

	// Spawner을 순회하면 Enemy을 Spawn
	// 이게 왜 스폰?
	NextSpawnerIndex %= EnemySpawners.Num();
	ATGEnemySpawner* Spawner = EnemySpawners[NextSpawnerIndex];
	NextSpawnerIndex++;

	// Spawn 성공 시에만 다음 Enmey로 넘어감
	if (Spawner && Spawner->SpawnEnemy(EnemyClass)) PendingSpawnIndex++;
}

void ATGWaveManager::FinishCurrentWaveSpawn()
{
	// Timer 정리
	GetWorldTimerManager().ClearTimer(SpawnTimerHandle);

	// 현재 웨이브 종료 처리
	bIsSpawning = false;
	const bool bCanStartNextWave = HasNextWave();

	// Wave Spawn 종료 이벤트
	OnWaveSpawnCompleted.Broadcast(ActiveWaveIndex, bCanStartNextWave);

	if (!bCanStartNextWave) return;

	// 웨이브 대기 시간 적용
	GetWorldTimerManager().SetTimer(
		NextWaveDelayTimerHandle,
		this,
		&ATGWaveManager::HandleNextWaveDelayFinished,
		DelayBeforeNextWave,
		false
	);
}

void ATGWaveManager::HandleNextWaveDelayFinished()
{
	StartNextWave();
}

void ATGWaveManager::InitializeSpawnState(int32 WaveIndex)
{
	// 새 웨이브 스폰 시작 상태 설정
	ActiveWaveIndex = WaveIndex;

	PendingSpawnQueue.Reset();
	PendingSpawnIndex = 0;

	if (!WaveDataList.IsValidIndex(WaveIndex)) return;

	UTGWaveData* WaveData = WaveDataList[WaveIndex];
	if (!WaveData) return;

	for (const FTGEnemySpawnGroup& SpawnGroup : WaveData->SpawnGroups){
		if (!SpawnGroup.IsValid()) continue;

		for (int32 i = 0; i < SpawnGroup.SpawnCount; ++i){
			PendingSpawnQueue.Add(SpawnGroup.EnemyClass);
		}
	}
}

bool ATGWaveManager::HasNextWave() const
{
	return WaveDataList.IsValidIndex(NextWaveIndex);
}
