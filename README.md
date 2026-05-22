# CYBER QUARANTINE

**FPS × Tower Defense Hybrid**  
실시간 FPS 전투와 그리드 기반 타워 디펜스가 결합된 하이퍼 액션 전략 게임입니다.

## 🚀 프로젝트 개요
- **장르**: 실시간 FPS + 타워 디펜스
- **엔진**: Unreal Engine 5.4 
- **비주얼 스타일**: '트론: 레거시' 스타일의 네온 사이버펑크
- **플랫폼**: PC (Windows)

## 🎮 핵심 게임 플레이
### 1. 실시간 모드 전환
- **전투 모드 (Combat Mode)**: 하이퍼 FPS 스타일의 기동성과 3종의 에너지 무기(Single, Shotgun, Repeater)를 활용한 직접 전투.
- **건설 모드 (Construction Mode)**: 그리드 기반의 실시간 벽 및 타워 배치. 게임 흐름 중단 없이 즉시 전략 수립 가능.

### 2. 건설 및 경제 시스템
- **3단계 건설**: 벽 배치 → 타워 설치 → 타워 업그레이드.
- **동적 경로 검증**: 적의 이동 경로를 완전히 차단할 수 없도록 실시간 알고리즘 적용.
- **자원 순환**: 적 처치 시 '에너지 코어'를 흡수하여 실시간으로 건설 비용 충당.

### 3. 적 및 보스전
- **다양한 유닛**: 지상, 공중, 돌격형, 방해형 등 5종의 일반 유닛.
- **Sentinel 보스**: 3단계 페이즈 변화와 고유 공격 패턴을 가진 거대 보스 레이드.

## 🛠 기술 스택
- **Language**: C++, Blueprints
- **Framework**: Enhanced Input System, Niagara VFX
- **Graphics**: Ray Tracing, Virtual Shadow Maps, Custom Depth Maps
- **Architecture**: 데이터 테이블 기반 스탯 관리, 옵저버 패턴(Delegate) 활용

## 🏆 승리/패배 조건
- **승리**: 최종 보스 'Sentinel' 처치
- **패배**: 에너지 코어(본진) 파괴 또는 플레이어 사망

---
*NBC 8기 Team 13 프로젝트*
