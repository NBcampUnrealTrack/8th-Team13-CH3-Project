# TRON-DEFENSE 프랍 원화 — Midjourney 최적화 프롬프트

## 미드저니 운용 기준

| 항목 | 설정값 | 비고 |
|------|--------|------|
| 버전 | `--v 6.1` | 멀티뷰 시트 표현에 가장 적합 |
| 스타일 | `--style raw` | MJ 자체 미화 필터 억제, 색감 의도대로 유지 |
| Stylize | `--stylize 200` ~ `--stylize 400` | 레퍼런스 시트는 낮게 — 너무 높으면 시트 레이아웃 무너짐 |
| 품질 | `--q 2` | 최고 품질 |
| 비율 | 오브젝트: `--ar 3:2` / 캐릭터: `--ar 2:3` / 시트: `--ar 16:9` | 용도별 상이 |
| 네거티브 | `--no text, watermark, logo, signature` | 모든 프롬프트 공통 |

### 미드저니 레퍼런스 시트 핵심 키워드
멀티뷰 시트 출력을 유도하는 검증된 표현:
```
game asset concept art sheet, multiple orthographic views,
front view, side view, back view, isometric perspective view,
clean dark background, design reference document,
character turnaround sheet (캐릭터 전용)
```

### 공통 아트 스타일 태그 (모든 프롬프트에 사용)
```
Tron Legacy aesthetic, cyberpunk neon noir,
matte obsidian black panels, electric cyan circuit lines,
by Syd Mead and Maciej Kuciara, game concept art, ultra-detailed
```

---

## 01. 에너지 코어 (Energy Core) — 본진

> `ATGCoreBase` | 플레이어가 방어해야 하는 목표 오브젝트

### 일반 원화 버전
```
/imagine prompt:
game asset concept art sheet, multiple orthographic views,
Energy Core structure, tower defense game base objective,
tall octagonal pylon, 3 meters high, diamond crystal sphere hovering at center,
matte obsidian black alloy frame, deep-cut glowing cyan circuit engravings,
four stabilizer legs with lit energy conduits, layered rotating transparent panels around crystal,
full health state: brilliant white-blue pulsing crystal, cyan glow pool on grid floor,
damaged state: cracked panels, flickering orange-red sparks, digital fragmentation,
front orthographic view, side profile view, top-down orthographic, isometric perspective (main large),
close-up detail callout of crystal core, damaged state variant shown,
Tron Legacy aesthetic, by Syd Mead and Vitaly Bulgarov,
cyberpunk neon noir, ultra-detailed, game concept art, dark background
--ar 16:9 --v 6.1 --style raw --stylize 300 --q 2
--no text, watermark, human figures, trees, natural elements
```

### BaseColor 버전
```
/imagine prompt:
game asset BaseColor texture sheet, flat unlit albedo reference,
Energy Core structure, tower defense base objective,
tall octagonal pylon, diamond crystal sphere at center,
flat solid colors only: frame #0A0A0A, crystal #AAFFFF, ring detail #FFFFFF, conduits #0055FF,
four stabilizer legs, layered transparent panels, orthographic technical illustration,
front view, side view, isometric view, top-down view,
flat 2D vector fill, zero shading, zero ambient occlusion,
neutral mid-grey background #808080, technical product sheet layout
--ar 16:9 --v 6.1 --style raw --stylize 50 --q 2
--no shadows, highlights, glow, bloom, ambient occlusion, specular, rim light,
text, watermark, gradient shading, lighting effects, emissive
```

---

## 02. 연사형 터렛 (Rapid-Fire Turret)

> `ATGWeaponTower` | 벽 장착형 기본 공격 타워 (공격력 10, 범위 500, 간격 2초)

### 일반 원화 버전
```
/imagine prompt:
game asset concept art sheet, multiple views,
wall-mounted rapid-fire turret, cyberpunk tower defense weapon,
compact dual-barrel turret on swivel joint, flat wall-top platform base,
sleek elongated twin barrels, glowing electric cyan muzzle tips,
matte black angular armor plating, glowing blue circuit trim lines,
rotating sensor dome behind barrels for target tracking,
energy conduit cables from body to wall-mount connector below,
idle state and firing state shown: bright cyan muzzle flash, electric blue energy rounds,
isometric perspective (main), front orthographic, side view, top-down with 240-degree firing arc,
detail callout: barrel mechanism close-up, wall connector piece shown separately,
Tron Legacy aesthetic, by Maciej Kuciara, ultra-detailed game asset, dark background
--ar 16:9 --v 6.1 --style raw --stylize 250 --q 2
--no text, watermark, human figures, natural elements
```

### BaseColor 버전
```
/imagine prompt:
game asset BaseColor texture sheet, flat unlit albedo reference,
wall-mounted rapid-fire turret, dual-barrel swivel design,
flat solid colors: body armor #0D0D0D, cyan trim lines #00FFFF, muzzle interior #AAFFFF,
conduit cables #0055FF, sensor dome #1A1A1A,
isometric view, front orthographic, side view arranged on sheet,
flat 2D vector fill, zero shading, technical product illustration,
neutral mid-grey background #808080
--ar 16:9 --v 6.1 --style raw --stylize 50 --q 2
--no shadows, highlights, glow, bloom, ambient occlusion, specular,
text, watermark, gradient, lighting effects
```

---

## 03. 범위 공격형 런처 (Area Attack Launcher)

> `ATGWeaponTower` AoE 파생 | 오렌지 계열 — 넓은 범위 타워

### 일반 원화 버전
```
/imagine prompt:
game asset concept art sheet, multiple views,
wall-mounted area-effect mortar launcher, cyberpunk tower defense weapon,
large single wide hexagonal barrel on heavy swivel mount,
glowing amber-orange interior barrel (#FF6600), imposing heavy silhouette,
large energy capacitor sphere on body side, charges brighter before firing,
matte black alloy panels with sharp angular orange accent lines,
firing state: glowing orange energy orb launching upward, wide shockwave ring on impact,
explosion radius shown as faint translucent orange dome projected on ground,
isometric perspective (main), top-down showing blast radius, side view,
charging state variant, firing state variant, wall connector piece separate,
Tron Legacy aesthetic, orange accent distinguishing from cyan turret,
by Vitaly Bulgarov, game concept art, ultra-detailed, dark background
--ar 16:9 --v 6.1 --style raw --stylize 300 --q 2
--no text, watermark, human figures, blue elements
```

### BaseColor 버전
```
/imagine prompt:
game asset BaseColor texture sheet, flat unlit albedo reference,
wall-mounted mortar launcher, heavy swivel mount, wide hexagonal barrel,
flat solid colors: body #0D0D0D, orange accent lines #FF6600,
barrel interior #FF9900, capacitor sphere #FFAA44, wall connector #1A1A1A,
isometric view, side view, top-down view arranged on sheet,
flat 2D vector fill, zero shading, technical product illustration,
neutral mid-grey background #808080
--ar 16:9 --v 6.1 --style raw --stylize 50 --q 2
--no shadows, highlights, glow, bloom, ambient occlusion, specular,
text, watermark, gradient, blue elements, lighting effects
```

---

## 04. 슬로우 필드 생성기 (Slow Field Generator)

> `ATGWeaponTower` 슬로우 파생 | 퍼플/틸 계열 — 속도 감소 타워

### 일반 원화 버전
```
/imagine prompt:
game asset concept art sheet, multiple views,
wall-mounted slow field generator, cyberpunk tower defense support weapon,
tall slender emitter body on wall mount,
three antenna prongs at top in triangle formation, each tipped with glowing teal-magenta node,
translucent hexagonal resonator prism at center body, soft purple-teal gradient light,
active slow field: large translucent interference dome, 4-tile radius, shimmering holographic patterns,
enemy inside field shown with faint purple desaturation aura,
particle wisps floating upward from resonator,
isometric perspective with dome visible (main), top-down showing field radius, side view,
active vs inactive state comparison, resonator chamber detail callout, wall connector piece,
Tron Legacy aesthetic, neon violet and teal color palette,
by Sparth, ultra-detailed game concept art, dark background
--ar 16:9 --v 6.1 --style raw --stylize 300 --q 2
--no text, watermark, orange elements, human figures
```

### BaseColor 버전
```
/imagine prompt:
game asset BaseColor texture sheet, flat unlit albedo reference,
wall-mounted slow field generator, tall slender body, three antenna prongs,
flat solid colors: body #0D0D0D, prong nodes #00FFCC, resonator prism #CC00FF,
field dome #9900CC at low opacity, wall connector #1A1A1A,
isometric view, side view, top-down with radius shown,
flat 2D vector fill, zero shading, technical product illustration,
neutral mid-grey background #808080
--ar 16:9 --v 6.1 --style raw --stylize 50 --q 2
--no shadows, highlights, glow, bloom, ambient occlusion, specular,
text, watermark, gradient, orange elements, lighting effects
```

---

## 05. 벽 구조물 (Wall Structure)

> `ABaseTower` | 경로 제어 기본 오브젝트, 타워 장착 플랫폼

### 일반 원화 버전
```
/imagine prompt:
game asset concept art sheet, multiple views,
defensive wall block, grid-aligned cubic structure, tower defense game,
solid cube 200x200x200cm, matte black carbon-composite panels,
glowing cyan circuit lines etched along all edges and corners,
flat mounting platform on top surface with turret connector port hole,
three game states shown side by side:
PREVIEW state: semi-transparent ghost outline, blue grid snap indicator,
INSTALLED state: solid full opacity, fully lit cyan circuits,
FOCUSED state: bright white-cyan rim glow on all edges when player aims at it,
2x1 connected row showing seamless block-to-block join,
3D isometric main view, top-down orthographic, front and side profiles,
Tron Legacy aesthetic, by Syd Mead, ultra-detailed game asset, dark background
--ar 16:9 --v 6.1 --style raw --stylize 250 --q 2
--no text, watermark, natural elements, characters
```

### BaseColor 버전
```
/imagine prompt:
game asset BaseColor texture sheet, flat unlit albedo reference,
defensive wall block, cubic grid-aligned structure,
flat solid colors: panel surface #0D0D0D, edge circuit lines #00FFFF,
top mount platform #1A1A1A, connector port #333333,
isometric view, front orthographic, side view, top-down view,
flat 2D vector fill, zero shading, technical product illustration,
neutral mid-grey background #808080
--ar 3:2 --v 6.1 --style raw --stylize 50 --q 2
--no shadows, highlights, glow, bloom, ambient occlusion, specular,
text, watermark, gradient, lighting effects
```

---

## 06. 기본 적 유닛 (Basic Enemy Unit)

> `ATGEnemyBase` | 오렌지 회로선, 코어로 돌진하는 보병형 적

### 일반 원화 버전
```
/imagine prompt:
character turnaround sheet, game asset concept art, multiple views,
basic enemy infantry, cyberpunk tron-style hostile program,
humanoid figure 170cm, lean hunched predatory silhouette built for swarming,
jagged shattered black digital armor plating, glowing amber-orange circuit lines (#FF6600),
full-face visor helmet with single horizontal orange eye slit, no visible face,
segmented tron-style joints with orange glow at articulation points,
front orthographic (main), back view, side profile, 3/4 isometric view,
idle A-pose, running pose, attack pose shown across sheet,
close-up helmet face detail, damaged state: cracked armor glitching orange lines, digital fragmentation,
Tron Legacy aesthetic, orange hostile vs blue player contrast,
by Maciej Kuciara and Aaron Limonick, ultra-detailed character concept art, dark background
--ar 16:9 --v 6.1 --style raw --stylize 350 --q 2
--no text, watermark, weapons in hand, blue elements, friendly appearance
```

### BaseColor 버전
```
/imagine prompt:
character turnaround sheet, BaseColor texture reference, flat unlit albedo,
basic enemy infantry, humanoid cyberpunk soldier,
flat solid colors: armor panels #111111, orange circuit lines #FF6600,
visor eye slit #FF9900, joint segments #1A1A1A, inner joint glow #FFAA44,
front view, back view, side profile, 3/4 view arranged on sheet,
flat 2D vector fill, zero shading, technical character sheet,
neutral mid-grey background #808080
--ar 16:9 --v 6.1 --style raw --stylize 50 --q 2
--no shadows, highlights, glow, bloom, ambient occlusion, specular,
text, watermark, gradient, lighting effects, blue elements
```

---

## 07. 적 스포너 게이트 (Enemy Spawner Gate)

> `ATGEnemySpawner` | 그리드 가장자리 적 출현 포탈

### 일반 원화 버전
```
/imagine prompt:
game asset concept art sheet, multiple views,
enemy spawn gate portal structure, tower defense grid edge,
imposing rectangular archway 3m wide 3m tall,
matte black reinforced alloy frame, glowing amber-orange energy conduits on all edges,
gate opening: swirling dark-purple and orange vortex of digital particles,
triangular apex at top with downward-pointing orange chevron warning marker,
status lights both sides: red active, dark dormant,
dormant state (main isometric), active spawning state with vortex at full intensity,
front orthographic, top-down, side profile, close-up vortex portal detail callout,
enemies mid-materialization visible in active state,
Tron Legacy aesthetic, orange-purple hostile palette,
by Ash Thorp, ultra-detailed game concept art, dark background
--ar 16:9 --v 6.1 --style raw --stylize 300 --q 2
--no text, watermark, blue elements, friendly structures
```

### BaseColor 버전
```
/imagine prompt:
game asset BaseColor texture sheet, flat unlit albedo reference,
enemy spawn gate portal, rectangular archway structure,
flat solid colors: frame #0D0D0D, orange conduit lines #FF6600,
chevron marker #FF4400, status lights #CC0000, vortex fill #660099,
isometric view, front orthographic, side view arranged on sheet,
flat 2D vector fill, zero shading, technical product illustration,
neutral mid-grey background #808080
--ar 16:9 --v 6.1 --style raw --stylize 50 --q 2
--no shadows, highlights, glow, bloom, ambient occlusion, specular,
text, watermark, gradient, lighting effects, blue elements
```

---

## 08. 플레이어 캐릭터 (Player Character)

> `ATGPlayer` | 전투 모드(bBuildMode=false) / 건설 모드(bBuildMode=true) 두 상태

### 일반 원화 버전
```
/imagine prompt:
character turnaround sheet, game asset concept art, multiple views,
player character protagonist, cyberpunk FPS tower defense hero,
athletic humanoid 185cm, sleek form-fitting black bodysuit,
bright electric cyan circuit lines (#00FFFF) tracing torso arms and legs musculature,
full-face angular helmet with wide horizontal cyan visor, face faintly visible through translucent visor,
COMBAT MODE left panel: energy pistol drawn right hand, dynamic aggressive stance, cyan at max intensity,
BUILD MODE right panel: builder gauntlet on left wrist projecting translucent grid hologram overlay,
front orthographic (main), back view, side profile, both mode poses,
close-up helmet detail, close-up gauntlet tool detail,
Tron Legacy warrior aesthetic, Tron Sam Flynn design reference,
by Sparth and Aaron Limonick, ultra-detailed character concept art, dark background
--ar 16:9 --v 6.1 --style raw --stylize 350 --q 2
--no text, watermark, cape, wings, orange elements
```

### BaseColor 버전
```
/imagine prompt:
character turnaround sheet, BaseColor texture reference, flat unlit albedo,
player character protagonist, cyberpunk bodysuit armor,
flat solid colors: suit #0D0D0D, cyan circuit lines #00FFFF,
visor #AAFFFF, gauntlet device #0055FF, joint accents #1A1A1A,
front view, back view, side profile, combat pose, build pose on sheet,
flat 2D vector fill, zero shading, technical character sheet,
neutral mid-grey background #808080
--ar 16:9 --v 6.1 --style raw --stylize 50 --q 2
--no shadows, highlights, glow, bloom, ambient occlusion, specular,
text, watermark, gradient, lighting effects, orange elements
```

---

## 09. 플레이어 무기 A — 에너지 피스톨 (Energy Pistol)

> `ATGWeaponPistol` | SINGLE_SHOT / REPEATER 지원

### 일반 원화 버전
```
/imagine prompt:
weapon concept art sheet, game asset multiple views,
cyberpunk energy pistol, handheld sidearm for FPS game,
sleek angular semi-automatic design 30cm length,
matte black composite body with sharp geometric cuts,
translucent energy chamber along barrel showing bright cyan plasma (#00FFFF),
hexagonal muzzle tip with glowing cyan core,
grip with cyan circuit engravings,
firing state: clean cyan energy bolt no smoke pure light,
right-side isometric (main), left-side view, top-down view,
first-person FPS grip view (camera eye POV, right hand holding),
firing state muzzle flash detail, energy chamber close-up,
Tron Legacy weapon aesthetic, by Viktor Antonov and Syd Mead,
ultra-detailed game weapon concept art, dark background
--ar 16:9 --v 6.1 --style raw --stylize 300 --q 2
--no text, watermark, scope, real-world weapon references
```

### BaseColor 버전
```
/imagine prompt:
weapon reference sheet, BaseColor texture flat unlit albedo,
cyberpunk energy pistol, handheld sidearm,
flat solid colors: body #0D0D0D, energy chamber #AAFFFF, circuit grip #00FFFF,
barrel #1A1A1A, muzzle #AAFFFF, trigger guard #222222,
right-side view, left-side view, top-down view, FPS grip view arranged on sheet,
flat 2D vector fill, zero shading, technical product illustration,
neutral mid-grey background #808080
--ar 16:9 --v 6.1 --style raw --stylize 50 --q 2
--no shadows, highlights, glow, bloom, ambient occlusion, specular,
text, watermark, gradient, lighting effects
```

---

## 10. 플레이어 무기 B — 에너지 디스크 런처 (Disc Launcher)

> 플레이어 보조 무기 | GDD "디스크 발사기" — 트론 아이덴티티 디스크 스타일

### 일반 원화 버전
```
/imagine prompt:
weapon concept art sheet, game asset multiple views,
energy disc launcher gauntlet, Tron identity disc inspired wrist weapon,
COLLAPSED state: compact wrist-worn armband gauntlet, dormant dim,
DEPLOYED state: gauntlet opens projecting spinning holographic disc, 25cm diameter,
disc itself: translucent flat ring layered energy rings bright cyan core, frisbee-like,
magnetic rail-track on back of gauntlet accelerates disc forward,
cyan energy trail as disc spins at launch,
gauntlet collapsed (wrist-worn), gauntlet deployed with disc spinning,
top-down view of disc alone showing ring layers,
first-person FPS throw pose, flight trail trajectory, ricochet impact state,
Tron Legacy identity disc aesthetic, Tron Sam Flynn disc combat reference,
by Ash Thorp, ultra-detailed weapon concept art, dark background
--ar 16:9 --v 6.1 --style raw --stylize 350 --q 2
--no text, watermark, medieval, organic shapes
```

### BaseColor 버전
```
/imagine prompt:
weapon reference sheet, BaseColor texture flat unlit albedo,
energy disc launcher wrist gauntlet, collapsed and deployed states,
flat solid colors: gauntlet body #0D0D0D, cyan trim #00FFFF, disc ring #AAFFFF,
disc core #FFFFFF, magnetic rail #0055FF, strap #222222,
collapsed state, deployed state, disc top-down view arranged on sheet,
flat 2D vector fill, zero shading, technical product illustration,
neutral mid-grey background #808080
--ar 16:9 --v 6.1 --style raw --stylize 50 --q 2
--no shadows, highlights, glow, bloom, ambient occlusion, specular,
text, watermark, gradient, lighting effects
```

---

## 11. 보스 캐릭터 (Boss Enemy)

> GDD Section 3 보스 레이드 | 15~20m 거대 메카 타입

### 일반 원화 버전
```
/imagine prompt:
character turnaround sheet, game asset concept art, multiple views,
colossal boss enemy mech titan, cyberpunk tower defense raid boss,
massive bipedal mechanical titan 15-20 meters tall,
heavily armored black plates with intense amber-orange glowing circuit networks,
WEAK POINTS: distinct glowing orange destructible panels on shoulders, chest, both knees,
each weak point shown intact and destroyed state (cracked dark sparking),
dual forearm-mounted disc-cannon arrays, charging orange plasma arcs between barrels,
legs wide enough to straddle multiple grid tiles simultaneously,
full-body front isometric (main, very large), side profile, back view,
close-up weak point detail callout: intact vs destroyed comparison,
scale comparison row: boss vs player character vs basic enemy,
idle standing pose vs attack charging pose,
damaged state: multiple weak points destroyed, locomotion erratic, digital glitch particles,
Tron Legacy mega-mech aesthetic, Pacific Rim scale reference,
by Vitaly Bulgarov and Karakter Design Studio, ultra-detailed, dark background
--ar 16:9 --v 6.1 --style raw --stylize 400 --q 2
--no text, watermark, organic elements, natural environment
```

### BaseColor 버전
```
/imagine prompt:
character turnaround sheet, BaseColor texture reference, flat unlit albedo,
boss enemy mech titan, colossal cyberpunk robot,
flat solid colors: armor panels #111111, orange circuit networks #FF6600,
weak point panels intact #FF4400, destroyed weak points #222222,
cannon barrels #1A1A1A, orange energy conduits #FF9900,
front view, side profile, back view arranged on sheet,
scale comparison: boss vs player vs enemy at same sheet,
flat 2D vector fill, zero shading, technical character sheet,
neutral mid-grey background #808080
--ar 16:9 --v 6.1 --style raw --stylize 50 --q 2
--no shadows, highlights, glow, bloom, ambient occlusion, specular,
text, watermark, gradient, lighting effects, blue elements
```

---

## BaseColor 버전 공통 파라미터

모든 BaseColor 프롬프트는 위 각 항목에 포함되어 있다.  
공통으로 적용되는 `--no` 태그 풀 목록:

```
--no shadows, highlights, glow, bloom, ambient occlusion, specular reflections,
rim lighting, caustics, lens flare, gradient shading, emissive effects,
lighting, depth of field blur, vignette, color grading, cinematic effects,
text, watermark, logo, signature
```

---

## 색상 팔레트 레퍼런스

```
[아군 / 플레이어]        [적 계열]               [슬로우 타워]
Body    : #0D0D0D       Body    : #111111        Prism   : #CC00FF
Circuit : #00FFFF       Circuit : #FF6600        Node    : #00FFCC
Fill    : #AAFFFF       Fill    : #FFAA44        Field   : #9900CC
Accent  : #0055FF       Accent  : #FF9900

[에너지 코어]            [스포너 게이트]           [보스 약점]
Frame   : #0A0A0A       Frame   : #0D0D0D        Intact  : #FF4400
Crystal : #AAFFFF       Conduit : #FF6600        Broken  : #222222
Ring    : #FFFFFF       Vortex  : #660099
```

---

## 프롬프트 사용 팁

**멀티뷰 시트가 제대로 안 나올 때**
> 같은 프롬프트로 4장 생성(U1~U4) 후 가장 잘 된 것 선택.  
> 또는 `--chaos 15` 추가해서 구도 변형 유도.

**특정 뷰만 원할 때**
> 뷰 목록 중 필요한 것만 남기고 나머지 삭제 후 `--ar 1:1` 또는 `--ar 4:3`으로 변경.

**BaseColor가 여전히 그림자를 생성할 때**
> `--stylize` 값을 `0`으로 낮추고,  
> 프롬프트 맨 앞에 `flat design illustration, solid fill colors,` 추가.

**스타일 통일이 안 될 때**
> 첫 번째로 마음에 드는 이미지 생성 후 해당 Job ID를 `--sref [jobID]`로 참조하면  
> 이후 모든 프롬프트에서 동일 스타일 유지 가능.
