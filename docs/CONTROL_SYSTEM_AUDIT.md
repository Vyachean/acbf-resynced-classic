# AC4 vs Resynced — complete control-system audit (work in progress)

**Priority P0 · November 15, 2026 · status: INCOMPLETE / GAMEPLAY HOOKS BLOCKED.**
Tracked in [issue #18](https://github.com/Vyachean/acbf-resynced-classic/issues/18).

**Scope:** primarily Xbox-style controller gameplay in *Assassin's Creed IV: Black Flag (2013)* vs. *Black Flag Resynced* (launch July 2026 public documentation, with **no validated 1.0.6-specific action priority**). Single-player is the active target. Modern-day and multiplayer portions of AC4, special activities and menus are inventoried as independent control surfaces so an input hook will never accidentally affect them.

The complete [machine-readable audit ledger](../research/control_system_ledger.json) currently contains **25 surfaces, 150+ action rows, 45 described/hypothesized transitions, 27 cross-context hazards, and 12 P0 unknowns**. These are an *inventory*, **not** 150 independently verified in-game behaviors.

## Stop-the-line rule

**No new gameplay input hook, physical-button remapper, action suppression or playable-mod claim until the control-system model is explicitly approved.** The provisional C++ \`resolve()\` and \`XActionBridge\` implement *design policies* in offline test fixtures, **not a reconstructed AC4 or Resynced input dispatcher**. All integration evidence defaults false. Do not make unverified assumptions true merely to activate a feature.

A working feature must preserve **all other actions sharing its input in all reachable contexts**, including holds, release, timed second-phase inputs, modifiers, equipment, swimming, menus and the ship. An unknown mode must be native pass-through.

## What must be modeled (not only an input table)

A candidate runtime model needs at least these dimensions:

| Dimension | Questions | Evidence available? |
| --- | --- | --- |
| Mode / control surface | Ground, combat, stealth, ledge, underwater, helm, spyglass, menu, special mode | Broad mode list documented; exact game state IDs **unknown** |
| Physical input | Xbox X/Y/A/B/LB/RB/LT/RT, sticks, D-pad, device-specific events | Default controls documented; actual device backend **unknown** |
| Input gesture | Press, hold/release, double-tap, modifier chord, rapid combo, stick direction | Many documented; threshold/ordering **unknown** |
| Player state | High/low profile, weapon, sprint, climb, hiding, aiming, combat | Concepts documented; exact detection APIs **unknown** |
| Target and environment | Highlighted target, distance, enemy phase, defense depleted, rail/wall, fall location | Design conditions documented; exact precedence **unknown** |
| Multi-stage commands | Counter → choice; kill → next kill; dodge → special attack; parry → takedown | Stage sequences documented; all timers and cancellations **unknown** |
| Feedback/exit | HUD prompt, rejected action, animation, interrupt, release ownership | Some indicators documented; executable state source **unknown** |
| Versions/settings | AC4 vs BFR TU 1.0.6/1.0.8; remapped input modes; Advanced Parkour/Manual Jump | Available feature/launch docs; owner's effective settings **unknown** |

This differs from a naive \`button -> action\` mapping. A B press after AC4's counter entry may be a **second-stage selection**, not an independent dodge, interaction, or falling command.

## 1. Original AC4: reconstructed high-level systems

### 1.1 Ground locomotion and traversal

AC4 has **Low Profile** and **High Profile** behavior. RT is a *mode modifier*, not just a bound sprint command. Movement plus RT starts running/free run, high-profile moves attract attention and may change available actions. Manual leap requires direction + High Profile + jump. The game also uses environment-aware automatic jumping, ledge attachments, catch-back, back/side ejects, swinging and contextual leap-of-faith; the game can stop a run at an unsafe edge. **RT + A cannot be treated simply as 'run + jump' in every context.**

Source: [Prima movement](https://primagames.com/eguides/assassins-creed-iv-black-flag-eguide/primer/on-foot-movement).

### 1.2 Ground combat — critical correction to current code assumptions

Original documented combat has several distinct phases:

1. **Neutral combat:** X attack, A break defense; target is selected by stick direction. Holding B blocks/parries incoming melee.
2. **Incoming enemy attack:** **tap B**, during a visible opponent attack warning, to initiate a counter. **Do not hold**, because hold produces ordinary guard.
3. **Simultaneous double counter exception:** when **two enemies telegraph attacks at once**, Prima describes an automatic double counter kill from the counter input alone, *without* the usual second-face-button choice. Exact eligibility and timing are unverified.
4. **Human shield:** while a gunner's attack warning is visible and a nearby enemy is available, **A** can seize a human shield instead of performing the usual guard break. An isolated A-remap could disable this defense.
5. **Counter-selection phase:** a successful counter opens a brief slow-motion state with **four face-button follow-ups**. A separate 2013 Prima [advanced tips article](https://primagames.com/tips/assassins-creed-4-black-flag-advanced-tips) states the Xbox mapping explicitly: **X = counter kill, B = throw, A = disarm/stun, Y = use selected tool**. The control names are documented, but **the exact engine timing window, input buffering, cancellation and second-press suppression are NOT established**.
6. **Kill streak:** after a combo kill or Counter Kill, direction plus an attack timed during the previous killing animation can continue the chain. It terminates on damage, target out of range, or other interrupt.
7. **Disengage:** High Profile can release the combat targeting stance and restore normal directional movement.

**Consequences:** the current provisional \`resolve_b()\` with a one-stage "counter if available, else dodge" policy is *not* an accurate implementation of original AC4 counter interactions. AC4's kill streak is also **not equivalent** to Resynced's separate Takedown/Chain Takedown system.

Source: [Prima original combat](https://primagames.com/eguides/assassins-creed-iv-black-flag-eguide/primer/combat); [original button guide](https://gamefaqs.gamespot.com/xbox360/705969-assassins-creed-iv-black-flag/faqs/68114). No internal game action timer, state or priority has been reverse engineered.

### 1.3 Stealth and contextual assassinations

AC4 assassin kills depend on at least **High/Low Profile**, camera/target highlighting and player geometry. Original guide separates low/high assassinations for standing, ledge, corner, hiding place; high-profile air assassination is position-dependent. Nonlethal takedown depends on **Unarmed mode**. Blending, wanted/detection, cover and citizen proximity change which moves are sensible. X can represent **ordinary melee, hidden-blade assassination, unarmed incapacitation, or continued kill streak**, but never assume the same gameplay-state arbitration is available in Resynced.

Sources: [Prima assassinations](https://primagames.com/eguides/assassins-creed-iv-black-flag-eguide/primer/assassinations), [Prima stealth](https://primagames.com/eguides/assassins-creed-iv-black-flag-eguide/primer/stealth).

### 1.4 Tools, water, ship, menus, present day

On foot: LT manually aims; RT is high-profile/firing in an aimed context; D-pad selects tools/inventory; tool use has a separate button. B interacts/pickpockets/picks up depending on object and gesture. Surface swimming, underwater swimming, ship helm, and gameplay on deck are **not the same input surface**.

Original ship controls: **A** increases speed; **B** decreases speed; **X** braces; **Y** operates swivel gun; **LT** aims; **RT** fires cannons; **LB** mortar; **RB** spyglass; D-pad shanties. Some actions are hold/release and cannon type also depends on aiming direction. Original official e-manual lists ship/ground/surface-water/underwater/**present-day** as separate categories. **No global X/B/A/RT mapping is permissible on the ship.**

Sources: [AC4 e-manual](https://dlassets-ssl.xboxlive.com/public/content/0044aef3-bd7e-4478-8782-6191546dcd9a/GameManual/240e30c4-b40a-4f93-9ae2-1c52ded445f7/en-US/index.html), [2013 ship controls](https://gamefaqs.gamespot.com/xbox360/705969-assassins-creed-iv-black-flag/faqs/68114?page=2). Source icons are sometimes inaccessible; ledger flags those bindings as **unverified** rather than guessing.

## 2. Resynced: public launch control surfaces and behavior

### 2.1 Ground locomotion, parkour and stealth

2026 published **default** (not evidence of unchanged 1.0.6 priority): **L3 sprint; A parkour up/jump; B parkour down/dive; X crouch; Y interact/assassinate/takedown; R3 lock target/Eagle Vision**. Resynced has optional **Advanced Parkour and Manual Jump**. According to Ubisoft, Advanced Parkour uses upward input to prioritize height gains, and downward input to control side eject/descent; Manual Jump affects direction and speed. Different settings can explain differing outcomes even with the **same** button layout.

Resynced can crouch in any place; social hiding, hood toggle, awareness indicators and contextual assassination remain separate states. Never equate AC4's RT profile with Resynced's L3 sprint alone.

Sources: [Shacknews launch mappings](https://canary.shacknews.com/article/149927/assassins-creed-black-flag-resynced-controls), [Ubisoft parkour/stealth](https://www.ubisoft.com/pl-pl/game/assassins-creed/news/2ud7PoyA7ctjHKKEtnH4Tf/assassins-creed-black-flag-resynced-road-to-launch).

### 2.2 Resynced combat is a separate action system

Published default: **RB normal attack / hold RB heavy attack; LB press parry / hold LB Observe; B dodge (double B roll); Y takedown and human shield; hold Y assassination**. **RB on a perch can hang an enemy below**, which is another distinct context of the ordinary attack button. **RT + X kick, RT + A sweep, RT + B quick rope dart**, **LT + D-pad equipment**, plus other tool combinations. The same face button under RT/LT can mean an ability instead of parkour. Several community reports explicitly warn that remapping RT sprint while leaving abilities on A/B can override parkour inputs; the exact arbitration on 1.0.6 remains unknown.

Ubisoft's combat design differs substantially from AC4:
- Ordinary attacks/heavy attacks remove an enemy's **defense**; exposed enemies can receive a **Takedown**.
- A **Perfect Parry** can create a Takedown / Chain Takedown opportunity; Chain Takedown reaches multiple eligible enemies depending on sword type.
- **Wall/Ground/Hidden Blade Takedowns** have different environmental/defense prerequisites.
- **Brute attacks are un-parryable** and need dodge/other responses; Perfect Dodge followed by RB has a special attack.
- Kick, Sweep, Rope Dart and Quick Shot can change enemy state, so a one-button "guard break" mapping cannot be decided from labels alone.

Sources: [Ubisoft combat breakdown](https://www.ubisoft.com/en-gb/game/assassins-creed/news/1A8XkbnvONciecEZm9my1C/assassins-creed-black-flag-resynced-combat-system-breakdown), [Shacknews defaults](https://canary.shacknews.com/article/149927/assassins-creed-black-flag-resynced-controls).

### 2.3 Ship, swimming, tools and UI are distinct surfaces

**At the helm:** A/B raise/lower sails (hold for all); X brace; RB swivel; LB mortar; RT blind/fire or LT+RT aimed weapon; Y mode change, boarding or Follow Sea depending on state. **In spyglass:** RT/LT zoom, A mark, B exit. **Swimming:** B hold dive, A hold rise, L3 underwater lunge or surface sprint. **On foot:** the very same buttons can mean attack, dodge, parkour or human shield. A safe mod must distinguish the surface **before** looking at the key.

Resynced also exposes **Press/Hold/Double Tap/Toggle**, controller remapping separated by gameplay contexts, adjustable analog deadzones, auto-switchable input devices, camera/lock-on and customizable HUD. This means input gestures and target cues can vary even without modifications.

Sources: [Prima controller tables](https://primagames.com/tips/assassins-creed-black-flag-resynced-complete-controls-pc-controllers), [Shacknews ship/swim tables](https://canary.shacknews.com/article/149927/assassins-creed-black-flag-resynced-controls), [Ubisoft control settings](https://www.ubisoft.com/en-gb/help/assassins-creed-black-flag-resynced/gameplay/article/customising-controls-in-assassin-s-creed-black-flag-resynced/000116769), [Ubisoft accessibility features](https://news.ubisoft.com/en-us/article/759plQmVmAvQySTmxx0r09/assassins-creed-black-flag-resynced-accessibility-spotlight).

## 3. 25 independent control surfaces: what is currently inventoried

See [JSON ledger](../research/control_system_ledger.json) for each action, raw input, gesture, source, unknown and transition. All engine priorities remain marked **UNKNOWN**.

| Group | AC4 contexts | Resynced contexts | Hook risk |
| --- | --- | --- | --- |
| Ground locomotion | Low / high profile, interaction | Sprint, crouch, interaction | Critical |
| Traversal | Auto free run, manual leap, climb, ledge, eject, fall | Up/down parkour, advanced modes, manual jump, dive | Critical |
| Combat | Neutral attack, incoming warning, tap/hold B, second-stage counter, kill streak, target selection | Attack/heavy, parry/dodge, perfect variants, takedown chain, target lock | Critical |
| Stealth | Blending, detection, high/low assassination, nonlethal, body handling | Crouch, Observe, human shield, assassination, interactions | Critical |
| Tools | Quick inventory, aiming, ranged fire, tool counter | LT/RT modifiers, equipment, quick tools/reload | Critical |
| Water | Surface swim/dive, underwater, resurface | Surface sprint, dive anywhere, swim up/down/lunge | Critical |
| Naval | Helm, guns, spyglass, boarding, shanties | Helm, mortar/swivel/ram, autopilot/pathfinder, boarding, shanties | Critical |
| Non-gameplay | Map, menu, photo mode | Map, HUD, photo mode, menus | Must pass through |
| Specialized | Abstergo present-day, original multiplayer, hunting/harpooning | Specialized mission and minigame controls need verification | Not to intercept |

The per-surface inventory is **not yet exhaustive**: exact gameplay-state IDs, precedence and 1.0.6-specific overrides remain unknown. "150+ action rows" is a catalog, not proof of completeness.

### Source-described exceptions to ordinary action flow

The ledger now includes **9 explicit control exceptions** where the same physical input can have different outcomes by combat prompt, timing, target position or gesture. This is not a game-engine state graph: all `engine_priority_verified` and `version_specific_verified` fields remain false. An exception that is not independently observable in the target build **blocks a consuming override** for that input.

The largest newly identified risks are AC4's *automatic double counter*, its *A human shield* during gunfire, Resynced *LB hold Observe* versus parry, and *RB perch hang-enemy* versus ground attack. These cases require no additional user gameplay test yet; they are source-backed research facts with unverified runtime arbitration.

## 4. High-risk conflicts (do not implement yet)

| Conflict | Potential silent regression | Required evidence |
| --- | --- | --- |
| **AC4 hold B guard / tap B counter → follow-up** | Loses guarding or second-stage counter choices | Exact state sequence and second-button behavior |
| **AC4 X attack / high-profile assassination / kill streak** | Breaks stealth and combo chain | Target eligibility, profile and animation phases |
| **AC4 RT high-profile / tool fire** | Replaces movement modifier with mere sprint | Distinguish movement, aiming and profile modes |
| **AC4 A human shield / guard break / jump** | Eliminates critical traversal action | Context/target/geometry gates |
| **Resynced RB press/hold/perch hang and Y press/hold** | Disables heavy strike, assassination, shield, interaction | Same-button gesture and action priority |
| **Resynced RT+A/B/X ability chords** | Blocks parkour / kick / sweep / quick rope | Full modifier conflict matrix per setting |
| **Resynced LB press parry / hold Observe** | Loses Observe or changes defensive timing | Distinct gesture/context, verified engine priority unknown |
| **Brute unparryable attacks** | Swallows the one defensive action that works | Enemy archetype, attack class and dodge eligibility |
| **Ship X/A/B/RB/LB/Y** | Breaks bracing, sailing, gunnery or boarding | Ship and sub-mode-specific input ownership |
| **Water A/B** | Blocks dive or ascent, creates unresponsive swimming | Water state / ledge vs dive transition |
| **Menus / specialized modes** | Soft-locks UI or side activity | Mandatory unknown-mode pass-through |

User **already observed** Attack and Takedown assigned to the same button producing Attack, not Takedown. This is evidence of that tested configuration, **not** proof that every eligible Takedown condition was present. No repeat is requested.

## Combat eligibility and exact-title-update boundaries

The [source-labelled combat condition register](COMBAT_CONDITIONS.md) and its [machine-readable matrix](../research/combat_conditions.json) identify a missing dimension in the earlier audit: **enemy archetype and attack type**.

- Original AC4 enemy guide distinguishes **Soldier, Scout, Gunner, Brute, Captain** and 10 move effectiveness categories (50 guide classifications). For **Brute**, the original counter input can select a **dodge rather than the counter follow-up**. The same guide is internally inconsistent about whether Captain can be affected by Break Defense, which is explicitly unresolved.
- The official **Resynced TU1.0.6** patch notes themselves document affected **vault→takedown, RT-sprint→parkour, rope dart interrupted→ability, combat→smoke assassination and enemy awareness→sweep** transitions. These are not general engine priority APIs or proof every bug is absent in the owner's installed binary.
- **TU1.0.7 / TU1.0.8** notes describe additional target-state/geometry/input-mode changes absent from TU1.0.6 documentation. Do not use newer mods or footage as authoritative behavior for TU1.0.6.

The previous `Snapshot` includes only global tri-state `can_counter/can_attack/can_takedown` flags, **without enemy archetype, attack type, prompt state, vaulting or interruptions**. It is thus *structurally insufficient* as a complete original/Resynced gameplay description; it must remain an offline hypothesis. No consuming game hook is permitted until the required state is verifiably available.

## 5. What remains to make this a trustworthy, complete model

The audit ledger deliberately lists P0 unknowns instead of filling them with guesswork:

1. AC4 follow-up button identities **are documented (X kill, B throw, A disarm/stun, Y tool)**; exact timing window, animation cancellation, interaction with hold-B guard, and priority on any simultaneous input remain unknown.
2. Original X precedence among combat hit, kill streak, high/low assassination, target geometry and nonlethal takedown.
3. Actual Resynced 1.0.6 configuration, input mode options, ability chords, parkour toggles, and target recognition.
4. Native pre-action eligibility and independent action dispatch for every desired new behavior, **not just Takedown**.
5. All reachable mode exits: swim→foot, helm→deck, combat→stealth/traversal, menu→game, ledge→water.
6. Uncataloged mission-specific, harpooning/hunting and interaction actions must remain native-pass-through until independently mapped.

Use [Prima original combat](https://primagames.com/eguides/assassins-creed-iv-black-flag-eguide/primer/combat) and [Ubisoft Resynced combat](https://www.ubisoft.com/en-gb/game/assassins-creed/news/1A8XkbnvONciecEZm9my1C/assassins-creed-black-flag-resynced-combat-system-breakdown) for the conceptual action-state descriptions. Neither source is a programming API or a native priority graph.

### Completion gate (issue #18)

- [ ] Every single-player gameplay surface and action in scope enumerated (or intentionally bypassed with explicit reason)
- [ ] Every overlapping physical input/chord audited for cross-surface priority, press/hold/release and multi-stage sequences (button mapping for AC4 counter established; timing/priority still unverified)
- [ ] Per-action evidence and version scope distinguish authoritative documentation, secondary references, user observations and engine-unknown behavior
- [ ] No original AC4 mechanic incorrectly equated to a similarly named Resynced action
- [ ] Independent target-build (1.0.6) behavior evidence sufficient for the exact **first intended override**
- [ ] Existing project \`resolve_b\`, \`resolve_a\`, \`resolve_x\` design assumptions reconciled with research or explicitly redesigned
- [ ] Any unknown / unsupported state provably falls back to native input, including ship, water, menu and minigame
- [ ] **Only then** approve a narrowly-scoped, reversible input/action adapter PR and a comprehensive one-session game test if needed

**The audit is not complete while these boxes remain unchecked. Do not ship gameplay interception.**
