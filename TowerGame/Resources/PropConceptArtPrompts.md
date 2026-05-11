# TRON-DEFENSE 프랍 원화 이미지 생성 프롬프트

## 공통 스타일 가이드 (모든 프롬프트에 적용)

**아트 디렉션 기준:**
- 영화 Tron: Legacy 사이버펑크 미학
- 배경: 거의 검정에 가까운 다크 톤 (#0A0A1A)
- 주 네온 컬러: 시안/일렉트릭 블루 (#00FFFF, #0080FF)
- 적 강조 컬러: 앰버/오렌지 (#FF6600, #FF9900)
- 재질: 매트 블랙 메탈 + 발광 회로 라인
- UE5 게임 에셋 레퍼런스 시트 형식 (정면/측면/투시도)

---

## 1. 에너지 코어 (Energy Core) — 본진

> `ATGCoreBase` | 방어 목표 오브젝트

### 원화 프롬프트 (영문)
```
Game asset concept art sheet, Tron Legacy aesthetic cyberpunk style.
OBJECT: Energy Core — the main base structure to defend in a tower defense game.
A tall, octagonal pylon-like structure, approximately 3 meters high.
Central element: a large, brilliant glowing sphere or diamond crystal hovering in the center,
pulsing with intense electric blue and cyan light (#00FFFF).
The outer frame is matte black obsidian-like alloy with deep-cut glowing circuit engravings.
Four stabilizer legs extend outward like a tripod base, each with lit energy conduits running upward.
The core sphere has layered transparent panels rotating slowly around it.
When at full health: brilliant white-blue glow. Damaged state: flickering, cracked panels with sparks.
Lighting: strong self-illumination, casting a wide cyan glow pool on the grid floor beneath it.
Reference sheet layout: 3D isometric view (main, large), top-down orthographic, side profile, close-up detail of the crystal core, damaged state variant.
Dark background. Ultra-detailed, Unreal Engine 5 PBR quality, no text, no watermark.
```

### 메모
- 플레이어가 보호해야 하는 본진이므로 시각적으로 가장 크고 인상적이어야 함
- HP가 낮을수록 코어가 깜빡이고 균열 이펙트가 추가되는 모습도 함께 표현

---

## 2. 연사형 터렛 (Rapid-Fire Turret)

> `ATGWeaponTower` (벽 위 장착형) | 기본 공격 타워

### 원화 프롬프트 (영문)
```
Game asset concept art sheet, Tron Legacy aesthetic cyberpunk style.
OBJECT: Wall-mounted Rapid-Fire Turret for a tower defense game.
A compact, aggressive turret mounted on a flat wall-top platform via a swivel joint.
Dual barrels, sleek and elongated, glowing cyan at the muzzle tips.
The turret body is matte black armor plating with sharp angular cuts and glowing blue trim lines.
A rotating sensor dome sits behind the barrels for target tracking.
Energy conduit cables connect the base to the wall mount below.
Firing state: bright muzzle flash, streaks of electric blue energy rounds.
Top view shows the radial firing arc (240 degrees).
Reference sheet layout: isometric perspective view (main), top-down orthographic, front view, side view, firing state, idle state, and detail of the barrel mechanism.
Wall mount base is shown separately as a compatible connector piece.
Dark background. Ultra-detailed, Unreal Engine 5 PBR quality, no text, no watermark.
```

### 메모
- 공격력: 10, 공격 범위: 500, 공격 간격: 2초 (코드 기준)
- 벽(ABaseTower) 위에 장착되는 구조이므로 하단에 벽 연결부가 표현되어야 함
- 업그레이드 레벨(0→1→2)에 따른 외형 변화 표현 권장

---

## 3. 범위 공격형 런처 (Area Attack Launcher)

> `ATGWeaponTower` 파생 (AoE 타입) | 넓은 범위 타워

### 원화 프롬프트 (영문)
```
Game asset concept art sheet, Tron Legacy aesthetic cyberpunk style.
OBJECT: Wall-mounted Area Attack Launcher for a tower defense game.
A large, single-barrel mortar-style launcher on a heavy swivel mount.
The barrel is wide, hexagonal cross-section, with a glowing orange-amber interior (#FF6600).
The body features a large energy capacitor sphere on the side that charges up (glows brighter) before firing.
Matte black alloy panels with angular orange accent lines distinguish it from the blue rapid-fire turret.
When firing: launches a glowing orange energy orb that expands into a wide shockwave ring on impact.
Explosion radius indicator: a faint translucent orange dome projected on the ground.
Reference sheet layout: isometric perspective view (main), top-down orthographic, side view, charging state (capacitor glowing), firing state (orb launch), and explosion impact top-down.
Wall mount base shown as compatible connector piece.
Dark background. Ultra-detailed, Unreal Engine 5 PBR quality, no text, no watermark.
```

### 메모
- GDD 기준 "범위 공격형 런처"
- 연사 터렛과 색상 구분: 오렌지 계열 강조색 사용
- 폭발 범위를 시각적으로 인지하기 쉽게 반투명 돔 표현 포함

---

## 4. 슬로우 필드 생성기 (Slow Field Generator)

> `ATGWeaponTower` 파생 (슬로우 타입) | 속도 감소 타워

### 원화 프롬프트 (영문)
```
Game asset concept art sheet, Tron Legacy aesthetic cyberpunk style.
OBJECT: Wall-mounted Slow Field Generator for a tower defense game.
A tall, slender emitter tower mounted on a wall, designed to project a suppression field.
The top features three antenna prongs arranged in a triangle, each tipped with a glowing teal/magenta node.
The body has a crystalline resonator chamber in the center — a translucent hexagonal prism emitting soft purple-teal light (#8800FF to #00FFCC gradient).
The slow field: a large translucent dome or cylinder of shimmering holographic interference patterns hovering around the tower's base, covering a 4-tile radius.
Enemies inside the field are shown with a faint purple desaturation aura indicating slowed state.
The generator hums softly — subtle particle wisps float upward from the resonator.
Reference sheet layout: isometric perspective view (main, including the field dome), top-down orthographic showing field radius, side view, active/inactive state comparison, and detail of resonator chamber.
Wall mount base shown as compatible connector piece.
Dark background. Ultra-detailed, Unreal Engine 5 PBR quality, no text, no watermark.
```

### 메모
- GDD 기준 "슬로우 필드 생성기"
- 퍼플/틸 계열 색상으로 다른 타워와 명확히 구분
- 필드 범위가 시각적으로 표현되어야 게임플레이에서 인지 용이

---

## 5. 벽 구조물 (Wall Structure)

> `ABaseTower` | 경로 제어 기본 오브젝트

### 원화 프롬프트 (영문)
```
Game asset concept art sheet, Tron Legacy aesthetic cyberpunk style.
OBJECT: Defensive Wall Block for a tower defense game, grid-aligned.
A solid cubic wall block (1x1 grid unit, approximately 200x200x200 cm).
Surface: matte black carbon-composite panels with glowing cyan circuit lines etched along all edges.
Top surface has a flat mounting platform — compatible with turret/launcher attachments (shown with a mount connector hole).
The wall glows more intensely at corners and edges.
States shown: (1) PREVIEW mode — semi-transparent blue ghost outline with grid snap indicator, (2) INSTALLED idle state — solid, fully glowing, (3) FOCUSED state — highlighted with bright white-cyan rim glow when player looks at it.
Reference sheet layout: 3D isometric view (main), top-down orthographic, four side profiles (front/back/left/right), preview state, installed state, focused state.
Shows how multiple wall blocks connect seamlessly in a row.
Dark background. Ultra-detailed, Unreal Engine 5 PBR quality, no text, no watermark.
```

### 메모
- 이미 `Gemini_Generated_Image_sx8nlisx8nlisx8n.png`에 기본 컨셉이 있음
- 이 프롬프트는 게임 상태(미리보기/설치/포커스)별 모습을 보완하는 용도

---

## 6. 기본 적 유닛 (Basic Enemy Unit)

> `ATGEnemyBase` | 코어를 향해 이동하는 기본 적

### 원화 프롬프트 (영문)
```
Game asset concept art sheet, Tron Legacy aesthetic cyberpunk style.
OBJECT: Basic Enemy Unit — ground infantry program corrupted in the grid world.
A humanoid figure, lightly armored, hunched and predatory in silhouette.
Height: ~170cm. Lean, fast-looking design — meant to swarm in groups.
Armor: jagged black shards of broken digital armor plating with glowing amber/orange circuit lines (#FF6600).
The orange glow clearly distinguishes them as hostile (opposite of player's blue).
Helmet: a visor-style face covering with a single glowing orange horizontal eye slit.
Limbs: slight tron-style segmented joints, subtle glow at articulation points.
Idle pose (A-pose or T-pose for game rigging reference), attack pose, and running pose shown.
Reference sheet layout: front view (main), back view, side profile, 3/4 isometric view, close-up face/helmet detail, idle / run / attack pose variants.
Damage state: cracked armor, glitching orange lines, digital fragmentation effect.
Dark background. Ultra-detailed, Unreal Engine 5 PBR quality, no text, no watermark.
```

### 메모
- 오렌지/앰버 컬러로 플레이어(블루)와 시각적 대비 확보
- 다수가 몰려다니는 상황을 고려해 실루엣이 명확해야 함
- 첫 번째 레퍼런스 이미지(`ChatGPT Image 2026년 5월 3일 오후 01_57_05.png`)의 소형 적 유닛 참고

---

## 7. 적 스포너 게이트 (Enemy Spawner Gate)

> `ATGEnemySpawner` | 웨이브 시작 시 적 출현 지점

### 원화 프롬프트 (영문)
```
Game asset concept art sheet, Tron Legacy aesthetic cyberpunk style.
OBJECT: Enemy Spawn Gate — a portal structure at the grid edge that enemies emerge from.
An imposing rectangular archway/portal, 3 meters wide and 3 meters tall, embedded in the grid border wall.
The gate frame is matte black reinforced alloy with glowing amber/orange energy conduits running along every edge.
The gate opening: a swirling vortex of dark purple and orange digital particles — enemies materialize from within.
When a wave begins: the vortex spins faster and brightens intensely before enemies emerge.
Two status lights on each side of the arch: red = wave active / incoming, dark = dormant.
Top of the arch has a triangular apex with a downward-pointing orange chevron — universal "enemy entrance" signal.
Reference sheet layout: isometric perspective view (main — dormant state), isometric perspective (active/spawning state), front orthographic, top-down orthographic, side profile, close-up of vortex portal detail.
Dark background. Ultra-detailed, Unreal Engine 5 PBR quality, no text, no watermark.
```

### 메모
- 그리드 가장자리(Entry Point)에 배치됨
- 오렌지/퍼플 컬러로 위험 지점임을 명확히 표시

---

## 8. 플레이어 캐릭터 (Player Character)

> `ATGPlayer` | FPS 주인공 — 전투 모드 / 건설 모드

### 원화 프롬프트 (영문)
```
Game asset concept art sheet, Tron Legacy aesthetic cyberpunk style.
OBJECT: Player Character — the protagonist of a hybrid FPS + tower defense game.
A sleek, athletic humanoid figure in a form-fitting digital armor suit.
Height: ~185cm. Silhouette should read instantly as "hero/player" vs enemy.
Armor design: close-fitting black bodysuit with bright cyan/electric blue circuit lines (#00FFFF) tracing the musculature.
Helmet: a full-face streamlined helmet with a wide horizontal visor glowing bright cyan. Slightly resembles Tron warrior aesthetic.
Right hand (COMBAT MODE): holding an energy pistol — see separate weapon prompt.
Left hand (BUILD MODE): holding a holographic construction tool — a glowing cyan data pad / builder gauntlet that projects translucent grid overlays.
Two mode variants clearly shown side by side:
  - COMBAT MODE: weapon drawn, dynamic athletic stance, blue glow at maximum intensity
  - BUILD MODE: builder gauntlet raised, construction mode HUD hologram projected from wrist
Reference sheet layout: front view (main), back view, side profile, combat pose, build pose, close-up helmet detail, close-up gauntlet/weapon detail.
Dark background. Ultra-detailed, Unreal Engine 5 PBR quality, no text, no watermark.
```

### 메모
- 코드에 전투 모드/건설 모드 전환 구현됨 (`bBuildMode` 플래그)
- 두 모드의 시각적 차이가 HUD 변화와 연동되므로 외형도 구분되어야 함

---

## 9. 플레이어 무기 A — 에너지 피스톨 (Energy Pistol)

> `ATGWeaponPistol` | 기본 플레이어 무기 (SINGLE_SHOT / REPEATER)

### 원화 프롬프트 (영문)
```
Game asset concept art sheet, Tron Legacy aesthetic cyberpunk style.
OBJECT: Player Energy Pistol — a handheld sidearm for a cyberpunk FPS game.
A sleek, angular semi-automatic pistol, roughly 30cm long.
Body: matte black composite material with sharp geometric cuts.
Glowing elements: a translucent energy chamber along the barrel showing bright cyan plasma inside.
The grip has cyan circuit engravings that pulse when firing.
Barrel tip: a hexagonal muzzle with a glowing cyan core.
Firing effect: clean cyan energy bolt with a brief trail — no smoke, pure light.
First-person view (as seen in FPS game — right hand hold, camera eye position).
Side view showing internal energy chamber.
Reference sheet layout: right-side isometric (main), left-side view, top-down view, first-person grip view (FPS camera angle), firing state (energy bolt exiting muzzle), and close-up detail of energy chamber and muzzle.
Dark background. Ultra-detailed, Unreal Engine 5 PBR quality, no text, no watermark.
```

### 메모
- `ATGWeaponPistol` 클래스 기반, `ShotInterval`로 연발 가능
- FPS 게임이므로 1인칭 그립 뷰가 필수

---

## 10. 플레이어 무기 B — 에너지 디스크 런처 (Disc Launcher)

> 플레이어 보조 무기 (GDD 언급: "디스크 발사기") | 트론 레거시 디스크 스타일

### 원화 프롬프트 (영문)
```
Game asset concept art sheet, Tron Legacy aesthetic cyberpunk style.
OBJECT: Energy Disc Launcher — a signature weapon inspired by Tron's identity disc mechanic.
A flat, circular launcher device held in one hand, approximately 25cm diameter when deployed.
Collapsed form: a compact wrist-worn gauntlet, resembling an armband. Dormant state = dim.
Deployed form: the gauntlet opens, projecting a spinning holographic disc of crackling cyan energy.
The disc itself: a translucent glowing flat ring (like a Frisbee), with layered energy rings and a bright cyan core.
Throw animation reference: the disc leaves a bright cyan trail as it ricochets off surfaces.
Launch mechanism: back of the gauntlet has a magnetic rail/track that accelerates the disc forward.
Reference sheet layout: gauntlet collapsed (wrist-worn), gauntlet deployed (disc formed and spinning), top-down view of the disc alone, first-person throw pose (FPS camera angle), flight trail trajectory, and impact/ricochet state.
Dark background. Ultra-detailed, Unreal Engine 5 PBR quality, no text, no watermark.
```

### 메모
- GDD: "디스크 발사기 등 하이테크 무기" 언급
- 개념 이미지의 "DISC LAUNCHER" 타워와 외형 언어를 공유하되 휴대용으로 차별화

---

## 11. 보스 캐릭터 (Boss Enemy)

> GDD Section 3 — 보스 레이드 | 거대 규모의 최종 적

### 원화 프롬프트 (영문)
```
Game asset concept art sheet, Tron Legacy aesthetic cyberpunk style.
OBJECT: Boss Enemy — a colossal corrupted program for the boss raid phase.
A massive, quadrupedal or bipedal mechanical titan, approximately 15–20 meters tall.
References: the large mech/boss from the first concept image (ChatGPT reference).
Body: heavily armored black plates with deep amber/orange glowing circuits — same orange as basic enemies but far more intense and complex.
Multiple WEAK POINTS shown as distinct glowing orange panels on the shoulders, chest, and legs — these are destructible and must be targeted by the player.
The boss has a secondary weapon: two forearm-mounted disc-cannon arrays that charge and fire sweeping orange energy beams.
Movement: designed for the grid — legs wide enough to straddle multiple grid tiles; wall structures are too small to block it.
Damaged state: destroyed weak point panels are cracked and dark, sparking with glitch particles; the boss's locomotion becomes more erratic.
Reference sheet layout: full-body front isometric (main — very large), side profile, back view, close-up of a destructible weak point (intact vs destroyed), scale comparison chart (boss vs player character vs basic enemy), and attack stance vs idle stance.
Dark background. Ultra-detailed, Unreal Engine 5 PBR quality, no text, no watermark.
```

### 메모
- GDD: "보스의 광범위 공격을 피하고 특정 부위를 파괴해야 하는 액션 중심의 전투"
- 약점 부위가 시각적으로 명확해야 게임 디자인 의도 반영 가능
- 첫 번째 레퍼런스 이미지의 대형 메카 보스를 기반으로 함

---

## 사용 권장 이미지 생성 도구

| 도구 | 용도 |
|------|------|
| **ChatGPT (DALL-E 4)** | 빠른 컨셉 검토, 여러 변형 시도 |
| **Midjourney v6+** | 고품질 최종 원화, 일관된 스타일 유지 |
| **Adobe Firefly** | 에셋 시트 형식, 다중 뷰 레이아웃 |
| **Gemini Imagen 3** | 구조적 도면 스타일 원화 |

## 공통 품질 태그 (모든 프롬프트 끝에 추가 권장)

```
ultra-detailed, 8K resolution, concept art, game asset reference sheet,
Tron Legacy aesthetic, cyberpunk, neon noir, dark background,
Unreal Engine 5 PBR material quality, professional game art,
no watermark, no text overlays
```

## 스타일 일관성을 위한 Midjourney 파라미터 예시
```
--style raw --ar 16:9 --v 6.1 --stylize 750
```
