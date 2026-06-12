# TowerGame 리팩토링 분석 보고서

> 작성일: 2026-06-13 / 분석 범위: `Source/TowerGame` 전체 (66개 클래스, 약 10,000줄)

## 총평

전체적으로 델리게이트 기반 이벤트 흐름(WaveManager → HUD → Widget)과 보스 페이즈/패턴 시스템은 잘 설계되어 있습니다.
반면 **플레이어/무기 계열에 복사-붙여넣기 중복**이 집중되어 있고, **타워의 매 틱 전체 액터 스캔** 같은 성능 문제,
그리고 리팩토링 중 반드시 같이 잡아야 할 **실제 버그 6건**이 발견됐습니다.

---

## 1. 버그성 코드 (우선순위 최상)

### ① 무기 교체 시 슬로우 디버프 타이머를 지워버림 — `TGPlayer.cpp:289`
`SwitchingWeapon()`의 교체 딜레이 람다가 `bCanSwitch` 복구와 함께 엉뚱하게 `SlowDebuffTimerHandle`을 Clear.
슬로우에 걸린 상태에서 마우스 휠로 무기를 바꾸면 **슬로우 해제 타이머가 사라져 이동속도가 영구히 느려진 채로 남는** 게임플레이 버그. 복붙 실수로 추정.

### ② `GetCurrentWeapon()`의 null 역참조 — `TGPlayer.cpp:535`
```cpp
TObjectPtr<UTGWeaponBase> WeaponPtr = OwnedWeapons.FindByPredicate(...)->Value;
```
`FindByPredicate`가 일치 항목이 없으면 nullptr을 반환하는데 곧바로 `->Value`를 역참조.
주석에는 "[FIX-PACKAGING] 역참조 전 유효성 체크"라고 적혀 있지만 실제 체크는 **역참조 후**에 수행. 데이터테이블 로드 실패 시 크래시.

### ③ `EquipWeapon()`의 반동 커브 복붙 버그 + 죽은 바인딩 — `TGPlayer.cpp:659~666`
- line 666: `RecoilCurveLoc.IsNull() ? CurveVector_None : AssetInfo.RecoilCurveRot` — **조건은 Loc을 검사하면서 값은 Rot을 사용**. Loc 커브만 없고 Rot 커브가 있는 무기는 회전 반동이 잘못 적용됨.
- line 659~663: 존재하지 않는 함수명 `OnAddWeaponOffset_Location`을 바인딩한 델리게이트를 만들지만 어디에도 쓰지 않음 (BeginPlay에서 이미 올바르게 바인딩). 삭제 대상.

### ④ NavigationManager null 체크 없는 역참조 — `TGMountedTower.cpp:53`
`ATGNavigationManager::Get(this)->NotifyBuildingPlaced()` — `Get()`이 nullptr을 반환할 수 있는데 바로 호출. 매니저 없는 레벨(튜토리얼 등)에서 타워 설치 시 크래시.

### ⑤ 적 사망 처리 중복 실행 가능 — `TGEnemyBase.cpp:193~212`
`TakeDamage`에 사망 플래그가 없어, 죽은 뒤 5초 수명 동안 물리 시뮬레이션 중인 `BodyParts`(PhysicsOnly 콜리전 유지)에 총알이 맞으면
`DestroyBodyParts()`와 **에너지 지급(`AddEnergy`)이 반복 실행**. 시체를 쏴서 에너지 무한 획득 가능. `bDead` 플래그로 차단 필요.
또한 `PartsLifeSpan` 프로퍼티(`TGEnemyBase.h:82`)가 있는데 `SetLifeSpan(5.0f)` 하드코딩에 밀려 미사용.

### ⑥ DebuffTower의 죽은 적 포인터 접근 — `DebuffTower.cpp:95~97`
`SlowedEnemies`(`TMap<ATGEnemyBase*, float>`)에서 적이 죽어 Destroy되면 raw 포인터 `Enemy &&` 체크만으로는 pending-kill 객체 접근을 못 막음.
`IsValid()` 체크 또는 `TWeakObjectPtr` 키로 변경 필요. `StopDebuff()`도 동일.

### (의도 확인 필요) 보스 사망 시 `HandleGameOver()` 호출 — `TGBossBase.cpp:233`
보스 처치는 승리일 텐데 `HandleGameClear()`(Result 상태)는 어디서도 호출되지 않고, `TGHUD::UpdateUIByState`에도 Result 상태 처리가 없음.
게임 클리어 흐름이 미구현인지 확인 필요. → **의도 불명확으로 이번 수정에서 제외, 추후 결정**

---

## 2. 성능 이슈

| 위치 | 문제 |
|---|---|
| `TGWeaponTower.cpp:163` | `DetectingEnemy()`가 Tick마다 `GetAllActorsOfClass` 호출 |
| `DebuffTower.cpp:37~45` | Tick마다 `DetectingEnemy()`(전체 스캔) + `ApplyRangeDebuff()`(SphereOverlap) + `SetRangeSphereScale()` 모두 실행 |

타워 수십 개 설치 시 매 프레임 `타워 수 × 전체 적 수` 비용 발생. 개선 방향:
- 탐지를 0.1~0.2초 타이머로 전환 (`ATGAttackEnemyBase`의 `PlayerDetectInterval` 타이머 방식과 패턴 통일)
- `ATGWaveManager`가 이미 스폰/제거 델리게이트로 적 수명을 추적하므로 **생존 적 목록을 WaveManager가 관리**하면 전체 스캔 제거 가능

부수 항목:
- ~~`DebuffTower::DetectingEnemy()`가 찾은 `Target`은 아무도 사용하지 않는 죽은 코드~~ → **정정**: `Target`은 부모 `ATGMountedTower::FollowTarget()`의 무기 메시 시각 추적에 사용됨. 삭제가 아니라 저비용 탐지로 전환 대상
- `DebuffTower`의 `SetRangeSphereScale()`은 BeginPlay 1회 + Upgrade 시 호출이면 충분
- `BuffTower.cpp:35~38`의 빈 `Tick()` 오버라이드 제거 가능
- `TGPlayer::Tick`은 매 프레임 LineTrace 2~3회 (InteractiveTrace + NPCTrace + WeaponTrace). `NPCTrace()`와 `InteractiveTrace()`는 같은 채널·같은 거리라 1회로 통합 가능

---

## 3. 코드 중복 (리팩토링 효과 최대 영역)

1. **무기 3종 `Shoot()` 복붙** — SingleShot/Shotgun/Repeater에서 "발사 이펙트→발사음→PlayRecoil→LineTrace→빔→착탄 이펙트→착탄음→데미지 분기" 시퀀스가 90% 동일.
   `UTGWeaponBase`에 `PlayMuzzleFeedback()` / `FireSingleTrace(SpreadDir)` 헬퍼 추출. 총기 바리에이션 추가 작업의 선행 작업.
2. **`FTickableGameObject` 보일러플레이트 중복** — SingleShot/Repeater가 각각 IsTickable/GetStatId/GetWorld 구현. 베이스로 일원화.
3. **`OwnWeapon()` switch 3케이스 복붙** — `TGPlayer.cpp:569~638`. 템플릿 헬퍼 하나로 통합 가능.
4. **데미지 적용 분기 중복** — "Enemy면 `ApplyDamage`, Boss면 `ApplyPointDamage`" 분기가 3개 무기에 반복. 모든 대상에 `ApplyPointDamage`로 통일하면 분기 제거.
5. **미니맵 마커 생성 코드 4벌** — `TGMiniMapWidget.cpp`의 마커 4종이 동일한 TextBlock 구성 반복. `CreateMarker(Text, Color, FontSize)` 헬퍼로 약 100줄 절감.
6. **보스 패턴들의 생성자 에셋 로드 중복** — 패턴 5종이 각자 `ConstructorHelpers`로 거의 같은 사운드/이펙트 로드. `UTGPatternBase`의 UPROPERTY로 이동 권장.

---

## 4. 구조/설계 개선

1. **하드코딩된 에셋 경로 의존 (약 30곳)** — `TGPlayerController`(입력 에셋 12개), `TGPlayer`(데이터테이블 3개+커브), 패턴 5종, `TGSingleGrid`, `TGPatternBase`.
   에셋 이동/리네임 시 런타임에 조용히 nullptr. `UPROPERTY(EditDefaultsOnly)` + BP 지정이 정석. `StaticLoadObject` 동기 로드는 히치 유발.
2. **수동 싱글톤 → WorldSubsystem** — `ATGWaveManager`/`ATGNavigationManager`의 static TWeakObjectPtr 싱글톤은 `UWorldSubsystem`으로 대체 시 월드별 수명 관리 자동화. (후순위)
3. **`ATGPlayer` 책임 과다 (772줄)** — 무기 인벤토리+반동(`UTGWeaponInventoryComponent`), 상호작용 트레이스(`UTGInteractionComponent`) 분리 권장. (후순위)
4. **자잘한 정리 항목**
   - `TGPlayer.h:188~193`: `BuildWidgetClass`/`BuildWidget` 멤버 미사용 (HUD로 이관됨) — 삭제
   - `TGPlayer.cpp:127~131`: `EndPlay`에서 `MarkAsGarbage()` 수동 호출 — 불필요·위험, GC에 위임
   - `TGPlayer.h:95~106`: `const int32 Getter()` → `int32 Getter() const` (EnemyBase Getter들도 동일)
   - `TGPlayer.cpp:671`: enum class를 Printf 가변인자에 직접 전달 — `static_cast<int32>` 필요
   - `UTGWeaponBase`의 `TraceHit`/`QueryParams` 멤버 상태 — 함수 지역 변수로 변경 권장
   - `TGPlayerWidget.cpp:99~103`: `NativeDestruct`에서 `OnEvadeChanged` RemoveDynamic 누락
   - 에너지 비용 하드코딩: 설치 50(`TGBaseTower.cpp:156`), 업그레이드 20(`TGMountedTower.cpp:125`) — UPROPERTY로 노출
   - 정상 동작 로그가 Warning 레벨 다수 (HUD, MiniMap, GameMode) — Log/Verbose로 하향
   - 주석 처리된 죽은 코드 블록 다수 — 삭제 (git이 기억)

---

## 권장 작업 순서

| 순서 | 작업 | 상태 |
|---|---|---|
| 1 | 버그 6건 수정 (섹션 1) | ✅ 완료 (2026-06-13) |
| 2 | 무기 시스템 중복 제거 (섹션 3-①~④) | ✅ 완료 (2026-06-13) |
| 3 | 타워 탐지 성능 개선 (섹션 2) | ✅ 완료 (2026-06-13) |
| 4 | 자잘한 정리 (섹션 3-⑤, 4-④) | ✅ 완료 (2026-06-13) |
| 5 | (후순위) Subsystem 전환, ATGPlayer 컴포넌트 분리 | 보류 |

## 에셋 경로 이전 (✅ 완료, 2026-06-13)

조사 결과 `ATGGameModeBase`가 `ATGPlayer::StaticClass()`/`ATGPlayerController::StaticClass()`를 직접 사용하는 등
**BP를 거치지 않는 경로가 존재**해, 하드코딩을 완전히 제거하는 대신 "생성자 디폴트 + UPROPERTY 노출(BP에서 교체 가능)" 방식으로 이전:

- **`TGPlayer` 무기 데이터테이블 3종**: 런타임 `StaticLoadObject`(동기 로드 히치 + 조용한 실패) 제거.
  `SingleShotWeaponTable`/`ShotgunWeaponTable`/`RepeaterWeaponTable` UPROPERTY(EditDefaultsOnly) 추가, 생성자에서 기존 경로를 기본값으로 지정.
  → **프로젝트에서 `StaticLoadObject` 0건.**
- **보스 패턴 사운드/이펙트**: 5개 서브클래스가 각자 로드하던 동일 에셋(`Enemy_Shot_00`, `P_Explosion`)을 `UTGPatternBase` 생성자 한 곳으로 통합 (11곳 → 1곳).
  `TGPlayerSequenceSpherePattern`은 원래 이펙트가 없었으므로 생성자에서 `AttackEffect = nullptr`로 기존 동작 보존.
  패턴별 BP에서 `Boss|Pattern|Sound`/`Effect` 카테고리로 개별 덮어쓰기 가능.
- **`TGPlayerController` 입력 에셋 12종**: 이미 UPROPERTY + 생성자 디폴트 구조(목표 상태) — 변경 불필요 확인.
- **`TGSingleGrid` 메시/머티리얼**: 컴포넌트 디폴트 설정(표준 UE 패턴)이고 "임시 시각화 ToDo" 주석이 있어 유지.

## 보류 항목 (후순위)

- **`UTGWeaponBase`의 `TraceHit`/`QueryParams` 멤버 상태 → 지역변수화**: 무기 리팩토링으로 사용처가 베이스 내부(`FireSingleTrace`)로 캡슐화되어 위험이 크게 줄어듦. 시그니처 연쇄 변경 대비 이득이 적어 보류.
- **보스 사망 시 `HandleGameOver()` 호출** (`TGBossBase.cpp`): 게임 클리어(`HandleGameClear`/Result 상태) 흐름 미구현으로 보임 — 팀 의도 확인 후 결정.
- **WorldSubsystem 전환, ATGPlayer 컴포넌트 분리**: 동작 변경 위험이 있어 위 작업 안정화 후 진행 권장.
