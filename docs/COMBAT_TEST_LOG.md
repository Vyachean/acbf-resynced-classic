# Combat test record — 2026-11-17

## Current result

**Gameplay result: NOT VERIFIED.** The reversible parry-window candidate launches with the current local 1.0.6 file set, but no controlled successful parry or counter has been observed. The earlier Cat Island attempts ended in desynchronization before a valid parry sample. Do not cite the offline simulator or CI as proof of game behavior.

## Candidate and rollback

- Candidate: additively packed `DataPC_boot_patch_02.forge`; current file SHA-256 `408A64C052E2CD440DA99D83C41BFDB119D6CB036C5E49B501EF54AB83DD500E`.
- The candidate changes one parsed field in `Edward - Parry` / `StateFightWindows[6]`: default window `130 → 200 ms`. Two conditional weapon overrides were already 200 ms and remain unchanged. The data was rebuilt and round-tripped; the game reached offline play after installation without crashing. These checks do **not** prove that the game loaded this resource or that it changes the player's counter window.
- Rollback is to remove only `DataPC_boot_patch_02.forge` from the game folder. Existing archives and the executable were not overwritten. No ASLR or other Windows security settings were changed.

## Why Cat Island is a bad test scene

The checkpoint puts a nearby guard close to Edward and gives little time to identify the attack cue and deliver a timed input. The player reported dying too quickly for this to be useful. Prior Q attempts ended in desynchronization without an observed parry, counter, or finisher; they are inconclusive, not failures of the candidate. Do not repeat this scene as the default test setup.

The live key-binding screen showed `Q` for Parry/Deflect and `B` for Auto Movement. A tutorial says red-glowing incoming attacks cannot be blocked and must be dodged with `Alt`; only a clearly non-red melee attack can serve as a parry sample. The red exclamation previously seen over the guard was an alert marker, not evidence of an incoming blockable attack.

## The existing “arena” is not a game arena

`acbf_combat_arena` is a console policy simulator for the documented original-AC4 guard/counter/follow-up model. It accepts labels such as enemy type and combat phase; it does not load the game, spawn an NPC, change health, inject controller input, or inspect Resynced state. No verified Resynced NPC-spawn API or safe arena-resource recipe was found. Do not modify save/world files or use guessed runtime addresses to manufacture a scene.

## Candidate ways to make a real test safer

- Use the game's **Forgiving** combat difficulty if available. [Ubisoft's launch combat description](https://news.ubisoft.com/en-us/article/gz5OxDe3cnPasLRvCLLup/assassins-creed-black-flag-resynced-deep-dive---expanding-on-the-original) says it reduces attack frequency and the number of enemies attacking Edward while surrounded. It is not invulnerability and does not by itself make Cat Island controlled.
- [Nexus #541](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/541) lists a separate 1.0.6 trainer file and advertises God Mode. [Nexus #616](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/616) lists a file labeled 1.0.6–1.0.7 and advertises invulnerability. Neither archive was downloaded, inspected, or run; downloads were gated behind Nexus sign-in in the available guest session. Do not bypass that gate or use mirrors.
- [ACBFRTools #661](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/661) advertises God Mode, but its stated compatibility fingerprint is `6F89418BE0DE287C7DD29893326EBFB817C3472C0106ECE8A6E8F81A915C3A55`; the locally pinned executable fingerprint is `19920F34BB2FAC814023EE3C27F0CECA1872E80A2AAFB37C092C419FFF77CC0D`. Its description says gameplay features are disabled on a different build unless the compatibility check is bypassed. That mismatch is not a safe TU 1.0.6 test path; do not enable its bypass against this executable without build-specific evidence.

## Offline verification performed

- `cmake --build build/cmake-combat-slice --config Release` — passed.
- `ctest --test-dir build/cmake-combat-slice -C Release --output-on-failure` — 7/7 passed, including the offline policy model, console arena demo, synthetic Puppet Native ABI test, and synthetic ASI smoke test.
- `python -m unittest discover -s tests -v` — 31/31 passed.

These are code/model/host checks only; none exercises the game combat loop.

## Next controlled test when work resumes

Get an authorized 1.0.6-compatible protection tool or a safe repeatable in-game encounter; inspect the exact artifact and compatibility before running it. Then use one ordinary soldier, a clearly non-red attack, and the same checkpoint/settings for baseline and candidate. Record cue, Q timing, animation/result, and health change. Stop if other enemies engage or health becomes critical. Restore baseline by removing only the candidate patch and compare the same encounter. Until that result exists, keep the candidate labeled `EFFECT_UNVERIFIED`.
