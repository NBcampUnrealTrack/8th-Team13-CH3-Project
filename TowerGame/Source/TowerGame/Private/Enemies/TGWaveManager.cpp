// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/TGWaveManager.h"

#include "EngineUtils.h"
#include "Enemies/TGEnemyBase.h"
#include "Enemies/TGEnemySpawner.h"

TWeakObjectPtr<ATGWaveManager> ATGWaveManager::Instance;

FTGEnemySpawnInfo::FTGEnemySpawnInfo() : EnemyClass(nullptr), SpawnCount(0)
{
}

FTGWaveInfo::FTGWaveInfo() : SpawnInterval(1.0f), DelayAfterSpawnCompleted(5.0f)
{
}

// Sets default values
ATGWaveManager::ATGWaveManager()
	: NextWaveIndex(0),
	ActiveWaveIndex(0),
	bIsSpawning(false),
	CurrentSpawnInfoIndex(0),
	SpawnedCountInCurrentInfo(0),
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

	// 현재 / 다음 웨이브 정보 갱신
	InitializeSpawnState(NextWaveIndex);
	NextWaveIndex++;

	// 웨이브 시작 이벤튼
	OnWaveStarted.Broadcast(ActiveWaveIndex);

	if (WaveInfos[ActiveWaveIndex].EnemySpawnInfos.Num() == 0){
		FinishCurrentWaveSpawn();
		return true;
	}

	bIsSpawning = true;

	// Spawn Timer 시작
	const float Interval = WaveInfos[ActiveWaveIndex].SpawnInterval;
	GetWorldTimerManager().SetTimer(
		SpawnTimerHandle,
		this,
		&ATGWaveManager::SpawnNextEnemy,
		Interval,
		true,
		0
	);

	return true;
}

void ATGWaveManager::SpawnNextEnemy()
{
	FTGWaveInfo& CurrentWave = WaveInfos[ActiveWaveIndex];
	while (true){
		// 모든 Enemy Spawn이 끝났는가
		if (!CurrentWave.EnemySpawnInfos.IsValidIndex(CurrentSpawnInfoIndex)){
			FinishCurrentWaveSpawn();
			return;
		}

		// SpawnInfo에서 지정된 수만큼 생성되었는가
		FTGEnemySpawnInfo& SpawnInfo = CurrentWave.EnemySpawnInfos[CurrentSpawnInfoIndex];
		if (SpawnedCountInCurrentInfo >= SpawnInfo.SpawnCount){
			if (!MoveToNextSpawnInfo()){
				FinishCurrentWaveSpawn();
				return;
			}
			continue;
		}

		// 해당 Enemy가 비어 있는가
		if (!SpawnInfo.EnemyClass){
			if (!MoveToNextSpawnInfo()){
				FinishCurrentWaveSpawn();
				return;
			}
			continue;
		}

		if (EnemySpawners.Num() == 0){
			FinishCurrentWaveSpawn();
			return;
		}

		// Spawner을 순회하며 Enemy Spawn
		NextSpawnerIndex %= EnemySpawners.Num();
		ATGEnemySpawner* Spawner = EnemySpawners[NextSpawnerIndex];
		NextSpawnerIndex = NextSpawnerIndex + 1;

		// Enemy 생성
		if (Spawner && Spawner->SpawnEnemy(SpawnInfo.EnemyClass)) SpawnedCountInCurrentInfo++;
		return;
	}
}

bool ATGWaveManager::MoveToNextSpawnInfo()
{
	// Enemy 생성에 사용할 인자 초기화
	CurrentSpawnInfoIndex++;
	SpawnedCountInCurrentInfo = 0;

	return WaveInfos[ActiveWaveIndex].EnemySpawnInfos.IsValidIndex(CurrentSpawnInfoIndex);
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
	const float Delay = WaveInfos[ActiveWaveIndex].DelayAfterSpawnCompleted;
	GetWorldTimerManager().SetTimer(
		NextWaveDelayTimerHandle,
		this,
		&ATGWaveManager::HandleNextWaveDelayFinished,
		Delay,
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
	CurrentSpawnInfoIndex = 0;
	SpawnedCountInCurrentInfo = 0;
}

bool ATGWaveManager::HasNextWave() const
{
	return WaveInfos.IsValidIndex(NextWaveIndex);
}
