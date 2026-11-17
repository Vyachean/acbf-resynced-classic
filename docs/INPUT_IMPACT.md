# Physical-button impact audit: what can an override silently disable?

**Status:** impact inventory only, independent of any game hook. The control system audit is
**INCOMPLETE_FEATURE_SCOPED**. See [#18](https://github.com/Vyachean/acbf-resynced-classic/issues/18).

The [complete source-labelled control ledger](../research/control_system_ledger.json)
is currently the project's best catalog of documented controller actions, but the
underlying game state priorities and some bindings are **UNKNOWN**. To prevent
an engineer from implementing "B = Counter" and accidentally deleting B's
ship/water/menu actions, [scripts/control_impact.py](../scripts/control_impact.py)
now derives a **conservative impact inventory** on demand.

## Reproducible queries (Python 3 stdlib, no game executable)

\`\`\`sh
python3 scripts/control_impact.py --game ACBFR --button X
python3 scripts/control_impact.py --game ACBFR --button B
python3 scripts/control_impact.py --game AC4 --button B
python3 scripts/control_impact.py --game AC4 --button RT
\`\`\`

Each command outputs JSON with:
- **exact_binding_actions**: all documented actions sharing that physical button
  across **every inventoried control surface**, including modifier chords;
- **unknown_binding_actions**: actions for which the source does **not** specify
  a physical button (potential collisions, never declared safe);
- **gesture and context**: press/hold/chord and action conditions, with original
  source identifiers for follow-up;
- **override_permission = IMPACT_REPORT_ONLY**: this report neither approves nor globally blocks an independent feature.

The analyzer is intentionally lexical. It recognizes face/shoulder buttons,
triggers, stick clicks, D-pad, View and Menu; \`RT + X\` contributes to both
physical buttons, and D-pad directions belong to the one D-pad family.
Unresolved \`one_of_four_face_buttons\` is treated as a **possible** X/Y/A/B
binding. A native game option cannot be ruled out from a missing menu-table
entry. **This is not a parser for Anvil/Resynced game files or a game
eligibility API.**

## High-risk examples, derived from the catalog

| Override hypothesis | Existing actions it could erase |
| --- | --- |
| Global Resynced **X** | Crouch on foot; **RT + X Kick**; **X Brace** at the Jackdaw |
| Global Resynced **B** | Dodge/roll; parkour down; water dive; sail lowering; leave spyglass; **RT + B** ability |
| Global Resynced **Y** | Interact; Assassinate; Takedown; Human Shield; ship boarding; Follow Sea |
| Global Resynced **RB** | Tap/hold normal/heavy attack; perched enemy hang; swivel guns at the helm |
| Global original **B** | Hold guard; tap counter; second-stage counter throw; foot interact/pickpocket; water dive; ship speed down |
| Global original **A** | Break defense; human shield on gunshot alert; jump/leap; raise ship speed |
| Global original **RT** | High Profile and free run; high-profile assassination; aimed firing; ship gunnery |

These are **warnings**, not exact priority rules. For example, the
original AC4's **Brute** counter event can become a dodge, while the
Resynced **Brute** requires avoiding un-parryable attacks. The
[combat-condition research](COMBAT_CONDITIONS.md) explains why names
and button assignments alone do not establish action equivalence.

## Engineering requirements before an actual override

1. Run the impact query for **every physical button** that the mod proposes
   to consume or reinterpret, including modifiers such as RT and LT.
2. Record the fate of **every catalogued action and every unresolved binding**:
   *native passthrough, independently verified contextual implementation, or
   feature explicitly out of scope with proved non-interference*. Simply
   declaring an action "not relevant" is not adequate evidence.
3. Identify and verify the target game build, input gesture, modifier, player
   state, target archetype, action readiness, animation phase and interrupt.
4. Test that releasing a claimed input restores all native long/double
   press semantics, and that ship, swimming, menu and unknown modes **never**
   consume an on-foot gameplay input.
5. Never use a green CI research test or a nonempty source table as proof
   that the game's internal priority or action eligibility API is verified.

The test suite [tests/test_control_impact.py](../tests/test_control_impact.py)
exercises the lexer, catches lost high-risk uses of X/B/A/RT/RB/Y, checks
ambiguous bindings cannot be silently excluded, and insists the analyzer
returns an impact-only verdict for every button in both games.

**Do not install or launch anything for this research artifact.** No
actual game-side input interception, code hooks, suppression, or new
packaged playable files exist.
