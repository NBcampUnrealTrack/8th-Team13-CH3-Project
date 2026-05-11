#include "BaseTower/TGResourceManager.h"

#include "EngineUtils.h"

// 싱글톤 인스턴스 초기화
TWeakObjectPtr<ATGResourceManager> ATGResourceManager::Instance;

ATGResourceManager::ATGResourceManager()
{
	// 리소스 매니저는 틱이 필요 없으므로 끔 (최적화)
	PrimaryActorTick.bCanEverTick = false;
}

void ATGResourceManager::BeginPlay()
{
	Super::BeginPlay();

	// 같은 월드에 2개 이상 존재하면 경고
	if (Instance.IsValid() && Instance.Get() != this && Instance->GetWorld() == GetWorld())
	{
		UE_LOG(LogTemp, Error, TEXT("TGResourceManager가 2개 이상 존재합니다."));
		return;
	}

	// 싱글톤 인스턴스 등록
	Instance = this;
}

void ATGResourceManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 이 인스턴스가 싱글톤이었다면 해제
	if (Instance.Get() == this)
	{
		Instance.Reset();
	}

	Super::EndPlay(EndPlayReason);
}

ATGResourceManager* ATGResourceManager::Get(const UObject* WorldContextObject)
{
	if (!WorldContextObject) return nullptr;

	UWorld* World = WorldContextObject->GetWorld();
	if (!World) return nullptr;

	//	등록된 인스턴스가 있고 같은 World 소속이면 그대로 사용
	if (Instance.IsValid() && Instance->GetWorld() == World)
	{
		return Instance.Get();
	}

	//	등록된 인스턴스가 없으면 현재 World에서 탐색
	for (TActorIterator<ATGResourceManager> It(World); It; ++It)
	{
		Instance = *It;
		return Instance.Get();
	}

	return nullptr;
}

void ATGResourceManager::AddResource(int32 Amount)
{
	// 유효하지 않은 양은 무시
	if (Amount <= 0) return;

	// 리소스 추가
	CurrentResource += Amount;

	//	리소스 변경 이벤트 발생 (UI 갱신)
	OnResourceChanged.Broadcast(CurrentResource);

	UE_LOG(LogTemp, Warning, TEXT("리소스 획득: +%d / 현재: %d"), Amount, CurrentResource);
}

bool ATGResourceManager::UseResource(int32 Amount)
{
	// 유효하지 않은 양은 무시
	if (Amount <= 0) return false;

	//	리소스 부족 시 실패
	if (CurrentResource < Amount)
	{
		UE_LOG(LogTemp, Warning, TEXT("리소스 부족: 필요 %d / 현재 %d"), Amount, CurrentResource);
		return false;
	}

	// 리소스 차감
	CurrentResource -= Amount;

	//	리소스 변경 이벤트 발생 (UI 갱신)
	OnResourceChanged.Broadcast(CurrentResource);

	UE_LOG(LogTemp, Warning, TEXT("리소스 사용: -%d / 현재: %d"), Amount, CurrentResource);
	return true;
}
