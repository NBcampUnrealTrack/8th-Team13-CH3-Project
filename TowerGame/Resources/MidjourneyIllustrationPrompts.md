# TRON-DEFENSE — Midjourney 고화질 일러스트 프롬프트

## 미드저니 공통 운용 기준

| 항목 | 설정 | 이유 |
|------|------|------|
| 버전 | `--v 6.1` | 현재 최고 품질, 세부 묘사 강점 |
| Raw 모드 | `--style raw` | Midjourney의 자체 미화 필터 억제, 의도한 색감 유지 |
| Stylize | `--stylize 750` ~ `--stylize 1000` | 트론 특유의 과장된 네온 비주얼에 적합한 예술적 해석 강도 |
| 화질 부스터 | `--q 2` | 최고 품질 렌더링 (시간 소요 증가) |
| Chaos | `--chaos 10` ~ `--chaos 25` | 구도 변형이 필요할 때만 사용 |
| Negative | `--no text, watermark, signature, username` | 모든 프롬프트에 공통 적용 |

### 색상 언어 통일 (Midjourney 인식 기준)
- **아군/플레이어**: `electric cyan`, `tron blue`, `#00FFFF`
- **적**: `amber orange`, `molten orange`, `#FF6600`
- **슬로우 타워**: `neon violet`, `teal magenta`
- **코어**: `white-blue core`, `pure luminous cyan`
- **환경/구조물**: `matte obsidian black`, `dark anthracite`

---

## 일러스트 목록

| # | 제목 | 용도 | 비율 |
|---|------|------|------|
| 01 | 키 아트 — 최후의 방어선 | 메인 포스터 / 타이틀 화면 | 16:9 |
| 02 | 전장 파노라마 — 그리드 전쟁 | 로딩 화면 / 인게임 배경 | 21:9 |
| 03 | 플레이어 — 전투 모드 영웅컷 | 캐릭터 선택 / 프로필 | 2:3 |
| 04 | 플레이어 — 건설 모드 | 튜토리얼 / 시스템 소개 | 16:9 |
| 05 | 적 군단 웨이브 | 웨이브 시작 연출 | 16:9 |
| 06 | 보스 인카운터 | 보스전 진입 컷씬 | 21:9 |
| 07 | 에너지 코어 수호 | 엔딩 / 게임오버 화면 | 16:9 |
| 08 | 타워 삼형제 | UI 아이콘 / 빌드 메뉴 배경 | 1:1 |
| 09 | 디스크 런처 FPS 시점 | 인게임 첫인상 / 트레일러 | 16:9 |
| 10 | 스포너 게이트 개방 | 웨이브 예고 연출 | 16:9 |

---

## 01. 키 아트 — 최후의 방어선

**장면 설명**  
플레이어가 혼자 에너지 코어 앞에 서서 수천의 적 군단을 마주하는 장면. 게임의 핵심 감정인 "고독한 방어자"를 담은 메인 포스터.

```
/imagine prompt:
lone cyberpunk warrior in a sleek tron-style glowing cyan bodysuit, standing on a dark grid battlefield,
facing an endless horde of amber-orange glowing enemy drones in the distance,
massive luminous energy core structure towering behind the player, pulsing white-blue light,
low angle heroic shot, player in foreground silhouette against the blinding core glow,
grid floor stretching to the horizon with glowing cyan lines, neon-lit defensive walls forming a maze pattern,
dramatic volumetric god rays from the core cutting through the darkness,
deep contrast: foreground black silhouette vs background massive orange army,
epic cinematic poster composition, rule of thirds, motion blur on enemy horde,
Tron Legacy film aesthetic, concept art by Maciej Kuciara and Syd Mead,
hyper-detailed, photorealistic game key art, Unreal Engine 5 cinematic render quality
--ar 16:9 --v 6.1 --style raw --stylize 900 --q 2 --no text, watermark, logo, ui elements
```

**창작 의도**
- 플레이어 실루엣은 어둡게, 코어 역광으로 윤곽만 표현 → 긴장감 극대화
- 오렌지(적)와 시안(아군 코어)의 보색 대비로 진영 구분을 한눈에
- 와이드 피사계심도로 배경의 적 군단은 흐릿하지만 숫자감 전달

---

## 02. 전장 파노라마 — 그리드 전쟁

**장면 설명**  
전투 한가운데의 그리드 전장 전체를 45도 상공에서 내려다보는 장면. 미로처럼 설치된 벽과 타워들, 이동하는 적들, 발사되는 포화가 동시에 표현된 전략적 긴장감.

```
/imagine prompt:
aerial bird's-eye cinematic view of a futuristic cyberpunk tower defense battlefield,
dark neon grid floor glowing with electric cyan (#00FFFF) grid lines,
maze of matte black defensive walls with glowing cyan circuit trim forming an L-shaped and U-shaped path,
three types of glowing turrets mounted on walls: rapid-fire cyan twin-barrel, orange arc-launcher, violet slow-field emitter,
dozens of amber-orange enemy units flowing through the maze corridors like liquid fire,
glowing energy projectiles mid-flight: cyan bolts from turrets, orange mortar arcs with trailing sparks,
large pulsing energy core structure at center-right, massive white-blue aura,
enemy spawn gates at grid edges glowing deep orange-amber, vortex portals wide open,
volumetric neon fog hanging low over the battlefield, cyan and orange light mixing in the mist,
ultra-wide cinematic panorama, IMAX composition, depth of field sharp in center blurring to edges,
Tron Legacy aesthetic, concept art by James Paick, game environment illustration, Sanctum 2 game reference,
photorealistic, Unreal Engine 5 Lumen quality, ultra-detailed
--ar 21:9 --v 6.1 --style raw --stylize 800 --q 2 --no text, watermark, hud, ui
```

**창작 의도**
- 게임의 타워 디펜스 요소(미로, 타워, 적 동선)가 모두 한 화면에 읽혀야 함
- 각 타워를 색상으로 구분해 유형을 한눈에 파악 가능하도록

---

## 03. 플레이어 — 전투 모드 영웅컷

**장면 설명**  
플레이어 캐릭터의 풀바디 영웅컷. 에너지 피스톨을 앞으로 겨누고 정면을 응시하는 자신감 있는 포즈. 트론 레거시의 전사 미학을 극대화.

```
/imagine prompt:
full body portrait of a futuristic cyberpunk warrior, Tron Legacy identity program aesthetic,
sleek form-fitting matte black bodysuit with bright electric cyan (#00FFFF) circuit lines tracing the torso and limbs,
full-face angular helmet with a wide horizontal cyan visor, faint face visible through translucent visor,
right hand raised, aiming a compact glowing energy pistol directly at viewer, muzzle glow cyan,
athletic powerful stance, slight three-quarter turn, weight on back foot,
background: dark abstract cyberspace grid environment with distant cyan light beams,
dramatic rim lighting from below with cyan edge glow outlining the entire figure,
studio hero shot, character concept art style,
by character concept artists at CD Projekt Red and Bungie, game character key art,
hyper-detailed armor surface showing micro circuit engravings, physically-based material quality,
cinematic focal length 85mm equivalent, slight depth of field on background
--ar 2:3 --v 6.1 --style raw --stylize 850 --q 2 --no text, watermark, wings, cape
```

**창작 의도**
- 캐릭터 선택 화면이나 로딩 화면에 바로 쓸 수 있는 수직 비율
- 뷰어를 직접 겨누는 포즈로 플레이어 이입감 강화

---

## 04. 플레이어 — 건설 모드 씬

**장면 설명**  
플레이어가 그리드 위에서 홀로그램 건설 인터페이스를 조작하는 장면. 왼손 건설 건틀릿에서 반투명 벽 홀로그램이 투영되고, 그리드에는 미리보기 상태의 청사진이 표시됨.

```
/imagine prompt:
cyberpunk soldier in tron-style black and cyan armor, kneeling on a glowing grid floor in construction mode,
left arm raised, wearing a glowing cyan gauntlet-bracer device projecting holographic blueprints,
large semi-transparent cyan hologram of a defensive wall structure floating above the grid, ghost blueprint effect,
grid floor tiles highlighting in bright cyan showing valid placement zones,
right hand lowered with energy pistol holstered, non-combat relaxed but focused posture,
multiple translucent UI readout panels floating around the player: grid coordinates, energy cost counters, path arrows,
atmospheric: soft cyan volumetric light from the hologram illuminating the player's helmet visor,
medium shot, slightly high angle showing the grid and the player together,
Tron Legacy aesthetic, Ghost in the Shell: SAC hologram aesthetic,
concept art by Sparth, game UX concept illustration, cinematic quality
--ar 16:9 --v 6.1 --style raw --stylize 750 --q 2 --no text labels, watermark
```

**창작 의도**
- 건설 모드의 기능(홀로그램, 그리드 하이라이트, 일시정지 느낌)을 한 컷에 설명
- 튜토리얼 화면이나 시스템 소개 이미지로 활용 가능

---

## 05. 적 군단 웨이브

**장면 설명**  
웨이브 시작 직전, 스포너 게이트에서 쏟아져 나오는 오렌지 빛 적 군단. 그리드를 가득 메우는 밀도감과 위압감이 핵심.

```
/imagine prompt:
massive horde of identical cyberpunk enemy soldiers, amber-orange glowing circuit armor, dark visor helmets,
pouring out from a towering rectangular portal gate structure at the far end of a dark neon grid,
the spawn gate blazing with deep orange-purple vortex energy, chevron warning lights flashing red,
enemy units filling every grid corridor, thousands deep, their orange glow lighting the dark floor from below,
leading enemies in foreground sharp and detailed, middle-distance soldiers blurred into a sea of orange light,
wide low angle looking up toward the horde, player's defensive walls barely visible at bottom of frame,
sinister and overwhelming atmosphere, predatory crowd movement,
Tron Legacy aesthetic, complementary colors (cyan grid vs orange enemies),
cinematic concept art by Karakter Design Studio and Blur Studio,
volumetric amber fog from the spawn gate, particle trails from fast-moving front runners
--ar 16:9 --v 6.1 --style raw --stylize 900 --q 2 --no text, watermark, friendly units
```

**창작 의도**
- 웨이브 시작 전 긴장감을 극대화하는 위협적 구도
- 하단에 플레이어의 방어선을 아주 작게 배치해 압도적 수적 열세 강조

---

## 06. 보스 인카운터

**장면 설명**  
거대한 오렌지 빛 보스 메카가 그리드를 가로질러 다가오는 장면. 플레이어는 전경에 조그맣게, 보스는 화면 전체를 압도하는 극적 스케일 대비.

```
/imagine prompt:
enormous corrupted cyber-mech boss titan, 20 meters tall, heavily armored black-plating with deep amber-orange glowing circuits,
four thick mechanical legs straddling multiple grid tiles, each footfall crushing the glowing grid lines,
multiple glowing weak-point panels on shoulders, chest, and kneecaps highlighted in intense burning orange,
forearm-mounted dual disc-cannon array charging up, arcing orange plasma crackling between the barrels,
tiny player figure in extreme foreground, cyan armor glowing defiantly, aiming upward at the massive boss,
dramatic forced perspective: player at 5% of frame height, boss filling 80% of frame height,
dark grid battlefield environment, destroyed defensive walls and debris scattered around the boss's feet,
volumetric orange shockwave expanding from boss's last footfall, cracking the grid surface,
cinematic low angle looking up, Dutch tilt 5 degrees, motion blur on boss's swinging arm,
epic scale, awe-inspiring, Tron Legacy boss fight aesthetic,
inspired by Shadow of the Colossus scale contrast, Pacific Rim mech design,
concept art by Vitaly Bulgarov and Aaron Beck
--ar 21:9 --v 6.1 --style raw --stylize 1000 --q 2 --no text, watermark, multiple players
```

**창작 의도**
- 보스전 진입 컷씬용 — 21:9 와이드스크린으로 압도적 스케일 극대화
- 약점 패널이 명확히 보이도록 하여 게임플레이 힌트 내포

---

## 07. 에너지 코어 수호

**장면 설명**  
에너지 코어가 적의 공격에 균열이 가며 마지막으로 빛나는 장면. 코어 주변에 마지막 타워 하나가 버티고 있고, 적들이 에워싸고 있는 긴박한 분위기.

```
/imagine prompt:
massive pulsing energy core structure, white-blue crystal diamond form, cracked and damaged,
flickering cyan light beams fracturing through the cracks, digital glitch fragmentation particles,
last standing rapid-fire turret on a damaged wall beside the core, still firing cyan bolts,
amber-orange enemy units swarming from all directions closing in, some already touching the outer perimeter,
dramatic volumetric light explosion from the damaged core illuminating everything in cold blue-white,
shattered grid tiles, broken wall fragments floating, emergency sparks and digital debris,
intense atmosphere of last stand and desperate defense,
overhead shot tilted 30 degrees, core at center, enemies spiraling inward like a vortex,
color grading: desaturated edges bleeding into intense white-cyan core center,
concept art tone: cinematic desperation, Tron Legacy meets Halo last stand imagery,
by Aaron Limonick and Ryan Church, game cinematic concept art
--ar 16:9 --v 6.1 --style raw --stylize 950 --q 2 --no text, watermark, player character
```

**창작 의도**
- 게임오버 직전 또는 클리어 직전의 감정선을 담은 장면
- 플레이어 캐릭터를 배제해 "코어 자체"가 주인공인 구도

---

## 08. 타워 삼형제

**장면 설명**  
세 가지 타워(연사 터렛, 범위 런처, 슬로우 필드 생성기)가 같은 그리드 벽 위에 나란히 배치된 장면. 각 타워의 개성과 기능적 아름다움을 표현하는 쇼케이스.

```
/imagine prompt:
three distinct cyberpunk tower defense weapons mounted side by side on dark matte-black grid walls, hero product shot,
LEFT: rapid-fire dual-barrel turret, glowing electric cyan muzzles, sleek aggressive silhouette,
CENTER: heavy wide-barrel area-effect launcher, amber-orange capacitor sphere charging on the side, imposing bulk,
RIGHT: tall slender slow-field emitter, three antenna prongs tipped with neon violet nodes,
    translucent violet-teal interference dome hovering below it showing effective radius,
each tower separated by one grid wall section, same base wall design for visual unity,
dark background with faint cyberspace grid, each tower subtly lit by its own color:
    left = cool cyan, center = warm amber, right = purple-teal,
square frame, centered product display, slight upward angle, dramatic studio lighting from above,
Tron Legacy material aesthetic, hyper-detailed surface circuit engravings on each tower,
concept art product showcase, by WETA Workshop concept design team
--ar 1:1 --v 6.1 --style raw --stylize 850 --q 2 --no text labels, watermark, human figures
```

**창작 의도**
- 정방형 비율로 UI 아이콘 배경이나 스팀 페이지 이미지로 활용
- 세 타워의 색상이 명확히 달라 빠른 시각 구분 가능하도록

---

## 09. 디스크 런처 FPS 시점

**장면 설명**  
플레이어 1인칭 시점에서 디스크 런처를 발사하는 순간. 회전하는 에너지 디스크가 화면 중앙을 가르며 날아가고, 전방에는 적들의 오렌지 실루엣이 늘어서 있음.

```
/imagine prompt:
first-person POV shooter perspective in a dark cyberpunk tron-style corridor on a glowing grid battlefield,
player's right arm visible in frame, wearing a sleek matte black and cyan armored gauntlet,
glowing cyan energy disc rotating rapidly on the launch rail, split second before release, crackling electricity,
cyan motion blur trails showing the disc's spin, bright white-cyan light core at the disc center,
looking forward down a neon-lit grid corridor, five amber-orange enemy silhouettes in the distance,
center of frame shows holographic crosshair / targeting reticle in cyan,
walls on both sides glowing with circuit patterns, grid ceiling above, perspective lines converging at vanishing point,
lens distortion slight fisheye, immersive FPS field of view,
atmospheric haze, volumetric cyan beam from disc illuminating the corridor,
inspired by Tron Legacy disc combat and Mirror's Edge visual clarity,
Doom Eternal / Titanfall FPS art direction, concept art by Viktor Antonov
--ar 16:9 --v 6.1 --style raw --stylize 800 --q 2 --no text, watermark, second weapon, crossbow
```

**창작 의도**
- FPS 게임의 핵심 경험인 무기 사용 순간을 1인칭으로 표현
- 트레일러나 스토어 페이지의 임팩트 컷으로 활용

---

## 10. 스포너 게이트 개방

**장면 설명**  
그리드 가장자리의 스포너 게이트가 활성화되며 최초 웨이브가 시작되는 순간. 게이트의 보텍스가 폭발적으로 열리며 적들이 처음 출현하는 드라마틱한 찰나.

```
/imagine prompt:
massive rectangular enemy spawn gate structure at the edge of a dark neon grid, activation moment,
gate frame: matte black angular alloy with fierce amber-orange energy conduits blazing at maximum intensity,
the gate portal exploding open: dark purple and burning orange vortex energy swirling into an outward burst,
first wave of amber-orange enemy soldiers materializing and stepping through the gate,
leading enemies in sharp focus, partially formed and glitching with digital fragmentation as they emerge,
bright orange light explosion from the gate casting long dramatic shadows across the grid floor,
two large red warning chevrons glowing on either side of the arch,
grid tiles near the gate cracking with energy discharge, sparks and particle shockwave radiating outward,
dramatic wide shot at ground level, gate filling the frame, grid stretching back toward viewer,
heavy atmosphere of dread and spectacle, cinematic event moment,
Tron Legacy portal aesthetic, concept art by Ash Thorp and Maciej Kuciara,
photorealistic render quality, volumetric explosion lighting
--ar 16:9 --v 6.1 --style raw --stylize 950 --q 2 --no text, watermark, friendly units, blue portals
```

**창작 의도**
- 웨이브 예고 연출 또는 튜토리얼 첫 웨이브 소개 이미지로 활용
- 처음 등장하는 적들이 반쯤 형성된 모습으로 "디지털 세계에서 소환"되는 느낌 강조

---

## 전체 스타일 일관성 유지 참고

### 모든 프롬프트에 공통 적용되는 레퍼런스 아티스트
- **Syd Mead** — 트론 레거시 오리지널 비주얼 디자이너
- **Maciej Kuciara** — 사이버펑크 2077 콘셉트 아티스트
- **Sparth (Nicolas Bouvier)** — Halo 시리즈 콘셉트 아티스트
- **Ash Thorp** — 디지털 사이버네틱 아트 전문가
- **Vitaly Bulgarov** — 메카닉/로봇 디자인 (Overwatch, Blizzard)

### 통합 색상 가이드 (Midjourney 언어)
```
아군 / 플레이어  → "electric cyan", "tron blue"
적              → "molten amber orange", "burning ember"  
슬로우 타워      → "neon violet", "ethereal teal"
에너지 코어      → "pure white-cyan", "luminous ice blue"
환경 구조물      → "matte obsidian", "dark anthracite metal"
배경 공간        → "deep space black", "void darkness"
그리드 라인      → "glowing cyan grid", "electric blue lattice"
```

### 장면별 카메라 앵글 요약
```
01 키 아트          : 로우 앵글 와이드샷, 역광 실루엣
02 전장 파노라마     : 45° 버즈아이, IMAX 와이드
03 플레이어 영웅컷   : 85mm 포트레이트, 약간의 아웃포커스
04 건설 모드        : 미디엄샷, 약간 높은 앵글
05 적 군단          : 로우 앵글, 위압적 클로즈업
06 보스 인카운터     : 익스트림 로우 앵글, 더치 틸트
07 코어 수호        : 오버헤드 30° 틸트, 코어 중심
08 타워 삼형제       : 정면 스튜디오 쇼케이스
09 디스크 FPS       : 1인칭 POV, 약한 피쉬아이
10 스포너 게이트     : 그라운드 레벨 와이드샷
```
