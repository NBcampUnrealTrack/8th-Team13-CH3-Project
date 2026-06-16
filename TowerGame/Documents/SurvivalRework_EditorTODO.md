# 뱀서식 개편 — 에디터 작업 체크리스트

> C++ 작업은 `survival-rework` 브랜치에서 진행 중. 이 문서는 **에디터(UMG/DataTable/BP/레벨)에서 사람이 직접 해야 하는 후속 작업** 모음이다.
> 완료한 항목은 `[x]`로 바꾸고, 필요 시 메모를 덧붙여 정리할 것.

---

## 빌드 관련 (먼저)
- [ ] **에디터 풀 빌드** 1회 필요 — 신규 UCLASS/USTRUCT 파일이 추가되어 Live Coding으로는 반영 불가.
- [ ] (권장) 프로젝트 파일 재생성(.uproject 우클릭 → Generate Visual Studio project files) — IDE에 신규 파일이 보이도록.

---

## Step 2 — 자원경제 → 경험치/건설토큰 (코드 완료, 에디터 정리 필요)
- [ ] **WBP_PlayerWidget**: 기존 `EnergyText`는 더 이상 바인딩되지 않음. 아래 위젯을 추가하면 C++가 자동 바인딩(전부 Optional이라 없으면 표시만 안 됨):
  - `LevelText` (TextBlock) — "Lv. N"
  - `PB_Exp` (ProgressBar) — 경험치 진행도
  - `BuildTokenText` (TextBlock) — 보유 건설 토큰 수
- [ ] **타워 플로팅 위젯(WBP)**: 기존 `EnergyText` → `BuildTokenText`(TextBlock, Optional)로 교체. `UpgradeCostText`는 "Cost: 1 Token"으로 표시됨(그대로 사용 가능).
- [ ] **블루프린트 그래프 점검**: `SpendEnergy` / `AddEnergy` / `GetCurrentEnergy` / `OnEnergyChanged` 노드를 쓰던 BP가 있으면 신규 API로 교체:
  - 적립: `AddExp(int)` / 소모·게이팅: `TryConsumeBuildToken(amount=1)` / 토큰추가: `AddBuildToken(int)`
  - 조회: `GetCurrentExp` / `GetCurrentLevel` / `GetExpToNextLevel` / `GetBuildTokens`
- [ ] (선택) GameMode BP에서 진행도/토큰 밸런스 조정: `BaseExpToNextLevel`, `ExpPerLevelGrowth`, `StartingBuildTokens`, `BuildTokensPerLevel`.

## Step 3 — 레벨업 무작위 특성 3택1 (코드 완료, 에디터 세팅 필수)
- [ ] **특성 DataTable 생성**: Row Struct = `FTGPerkData`. 행마다 `PerkId / DisplayName / Description / Icon / Effect / Magnitude / Weight` 입력.
  - Effect 종류: `MaxHpUp`(체력증가량) · `Heal`(회복량) · `MoveSpeedUp`(0.1=+10%) · `WeaponDamageUp`(0.15=+15%) · `EvadeCountUp`(횟수) · `GrantBuildToken`(토큰 수)
- [ ] **GameMode BP**: `PerkComponent → PerkTable`에 위 DataTable 지정. (`NumChoices` 기본 3)
- [ ] **WBP_PerkSelect 제작**: `UTGPerkSelectWidget` 상속.
  - `OnShowPerks(Options)` 구현 → 카드 3장 생성/표시 (각 카드에 DisplayName/Description/Icon)
  - 카드 버튼 클릭 → `SelectPerk(Index)` 호출
  - `OnHidePerks()` 구현 → 카드 닫기
- [ ] **HUD BP**: `PerkWidgetClass`에 위 WBP_PerkSelect 지정.
  - ⚠️ PerkTable에 특성이 있는데 PerkWidgetClass가 비어 있으면 레벨업 시 **일시정지 상태로 소프트락**되니 반드시 둘 다 세팅.
- [ ] (확인) 레벨업 시 게임이 일시정지되고 커서가 나타나며 카드 선택 후 재개되는지 테스트.

## Step 4 — 적 경량화(경량 Actor, 수백 마리) (코드 완료, 에디터 세팅 필수)
- [ ] **BP_SwarmEnemy 제작**: `ATGSwarmEnemy` 상속.
  - `MeshComp`에 **StaticMesh 에셋 지정 필수** — 메시가 없으면 보이지도, 무기 트레이스(ECC_Visibility)에 맞지도 않음.
  - 스탯 조정: `MoveSpeed / AttackRange / AttackDamage / AttackInterval`, 전투는 `Combat`의 `MaxHP / ExpDropAmount`.
  - (선택) `DeathEffect`(Niagara) / `DeathSound` 지정.
- [ ] **레벨에 `ATGSwarmSpawner` 배치**: `EnemyClass`=BP_SwarmEnemy, `SpawnCount/SpawnInterval/SpawnPerTick/SpawnRadius` 설정. `bAutoStart`로 자동 시작 or BP에서 `StartSpawning()` 호출.
- [ ] (확인) 코어를 향해 진격 → 사거리 내 코어 공격 → 플레이어 사격으로 처치 시 경험치 획득(레벨업 연동)되는지 테스트.

### Step 5 — 타워 타겟팅 일원화 (코드 완료, 별도 에디터 작업 없음)
- 타워가 이제 `ITGTargetable` 인터페이스 + `NavigationManager` 타겟 레지스트리로 경량 적을 조준/공격/슬로우함. 경량 적은 스폰 시 자동 등록되므로 **추가 에디터 세팅 불필요**.
- 참고: 구 `ATGEnemyBase` 적은 이 레지스트리에 등록되지 않으므로 더 이상 타워의 타겟이 되지 않음(폐기 전제대로). 구 적을 계속 쓰려면 별도 처리 필요.
- [ ] (확인) 무기 타워가 경량 적을 자동 조준·사격, 디버프 타워가 범위 내 경량 적을 감속시키는지 테스트.

### Step 6 — 경량 적 경로 이동 (코드 완료, NavMesh 필요)
- 경량 적이 NavMesh 경로로 코어까지 이동(타워/벽 우회). AIController/CharacterMovement 없이 주기적 경로 재탐색(`RepathInterval` 기본 2초, 적마다 분산) + 경유점 추종. 경로 실패 시 직진 폴백.
- [ ] **레벨에 NavMeshBoundsVolume 존재 + 빌드 확인** (필수). 없으면 경로를 못 찾아 직진만 함. (구 적도 NavMesh를 썼으니 보통 이미 있음)
- [ ] (튜닝) BP_SwarmEnemy의 `RepathInterval`/`WaypointAcceptanceRadius`로 반응성·부하 조절. 적이 매우 많으면 `RepathInterval`을 늘려 경로탐색 부하를 낮춤.
- [ ] (확인) 타워/벽 사이로 경량 적이 우회해 코어로 접근하는지 테스트.

### Step 7 — 경량 적 분리(겹침 방지) (코드 완료, 별도 에디터 작업 없음)
- 경량 적이 서로 겹치지 않게 밀어내는 분리력 추가. `UTGSwarmFlockSubsystem`(월드 서브시스템, 자동 생성)이 프레임당 1회 공간 해시를 만들어 이웃을 빠르게 찾음. **추가 에디터 세팅 불필요.**
- [ ] (튜닝) BP_SwarmEnemy의 `SeparationRadius`(밀어내는 반경)·`SeparationWeight`(퍼짐 강도)로 뭉침 정도 조절. 0으로 두면 분리 끔.
- [ ] (확인) 적들이 한 점에 포개지지 않고 적당히 퍼져 이동/코어 포위하는지 테스트.

## Step 8 — 플레이어 위젯/건설 UX 재설계 (코드 완료, 에디터 작업 필수)
요약: HUD를 단일 위젯으로, 미니맵/총기표시/빌드모드 토글 제거. 하단 퀵슬롯(1=미선택, 2=벽, 3+=타워)을 **Tab**으로 순환, **E**로 설치(사격은 좌클릭 그대로). 벽→터렛 2단계 유지, 조준 칸 반투명 프리뷰.
- [ ] **입력 IA_CycleBuildSlot 생성**: `/Game/Player/Input/Actions/IA_CycleBuildSlot` (Digital/bool). IMC_Player에 **Tab** 키로 매핑. (C++가 이 경로를 자동 로드 — 만들기 전까지는 "에셋 못 찾음" 경고가 뜨고 슬롯 전환이 작동 안 함)
- [ ] (정리) IMC_Player에서 기존 빌드모드 토글(IA_Build), 숫자키 타워선택(IA_BuildTowerSelect) 매핑 제거 — 더 이상 사용 안 함.
- [ ] **WBP_PlayerWidget 단일 HUD로 재구성**:
  - 상시: `HP_Bar`, `CoreHP_Bar` (ProgressBar, Optional), `Evade_BarGroup`(HorizontalBox, Optional)
  - 진행도: `LevelText`, `PB_Exp`, `BuildTokenText`
  - (선택) `Txt_CurrentWave`, `Txt_BossName`, `PB_BossHP`, `Txt_EnemyType`, `PB_EnemyHP`
  - **제거**: `WeaponImage`(현재 총기 표시), `MiniMapWidget`(미니맵)
  - **퀵슬롯 바**: `OnBuildSlotUpdated(SelectedIndex, SlotCount)` 이벤트를 구현해 하단 슬롯바를 그림. 슬롯 의미: 0=미선택, 1=벽, 2.. = 타워(WeaponTower/DebuffTower/BuffTower 순). `SelectedIndex` 슬롯을 강조.
- [ ] **HUD BP**: 빌드 위젯 관련 설정 제거됨(C++에서 `BuildWidgetClass` 삭제). `PlayerWidgetClass/PauseWidgetClass/GameOverWidgetClass/PerkWidgetClass`만 지정.
- [ ] (선택) 고아가 된 `WBP_BuildWidget`, `WBP_MiniMap` 삭제.
- [ ] (선택) 벽 베이스 머티리얼에 `PreviewColor` 벡터 파라미터 추가 → 설치 가능=녹색/불가=빨강 고스트 프리뷰 색 반영.
- [ ] (확인) Tab으로 슬롯 순환 → 벽 슬롯에서 빈 칸 조준 시 반투명 프리뷰 → E로 벽 설치 → 타워 슬롯으로 벽 조준 후 E로 터렛 장착. 좌클릭 사격은 항상 동작.

### 남은 한계 (다음 단계에서 보완)
- [ ] **미니맵 미표시**: 미니맵 등록 경로가 `ATGEnemyBase` 기준(경량 적 미표시). (이번 재설계로 미니맵 자체를 제거함)
- [ ] **경로탐색 최적화**: 현재 `FindPathToLocationSynchronously`(쿼리마다 UNavigationPath UObject 생성). 수천 마리면 `FindPathSync`(UObject 미생성) 또는 플로우필드로 전환 권장.
- [ ] **보스/일반 적 미전환**: `UTGCombatComponent`를 아직 `ATGEnemyBase`/보스에 적용 안 함(리스크 관리). 추후 공용화로 중복 제거.
- [ ] **렌더링**: 진짜 수천 마리까지면 InstancedStaticMesh/Mass로 추가 최적화 필요(현재는 개별 Actor, 수백 마리 목표엔 충분).
