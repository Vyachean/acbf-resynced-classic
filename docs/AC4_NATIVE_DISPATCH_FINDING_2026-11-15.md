# Original AC4 static input-to-command path (2026-11-15)

## Scope and confidence

**Game:** locally installed original Assassin's Creed IV executable; exact release unknown. The sample is modified/unverified.  
**Method:** read-only Ghidra MCP decompilation and call/xref traversal of the existing local program. Decompiler output is reconstructed pseudocode, not source.  
**Status:** confirmed by static analysis only. The game was not launched; dynamic behavior is unconfirmed.

## Recovered path

The native input manager maintains previous/current logical-control state, checks active context filters, and exposes a rising-edge query. In an assassin-behavior callback path, a logical control edge is considered only after callback/object suppression, prior-result exclusions, an actor/context predicate, and an input-consumption check. A second mode-dependent logical slot is checked with its own edge and timing gates.

Those validated flags are passed into a candidate resolver. The resolver checks candidate sources in an explicit fallback order and also evaluates an actor/target candidate path. These paths compare current component/mode state and candidate/target handles; one target path additionally calls geometry/visibility-like validators. The resolver returns candidate metadata and a command/state discriminator.

When the corresponding input flag is set, a typed dispatcher branches on the resolved candidate type and forwards to action-management helpers. The forwarding path creates or updates an engine message object and submits it to the message system. This establishes a real static path:

**logical input edge → context/consumption/state gates → ordered actor/target candidate resolution → typed command routing → engine message submission**

The behavior callback is registered in a subsystem whose binary type key resolves to BhvAssassin; this does not prove that every associated actor is the playable character.

## What is and is not established

- **Confirmed statically:** a logical input edge and actor state can reach conditional candidate resolution and a typed engine command/event submission.
- **Confirmed statically:** XInput's logical slot 3 is populated from the B bit in the inspected mapping path. DirectInput uses named profile binding slots, so this slot number is not a universal physical-button identity. The alternate slot is mode-dependent.
- **Not established:** the exact retail build, runtime provider in the modified installation, player-only ownership, semantic names of the resolved command discriminators, whether they mean attack/counter/finish/assassination, exact hold windows, interruption/cancellation, or observed gameplay results.
- This does not establish the requested X-button Attack-versus-Finisher priority or the B counter/follow-up rule. It is a concrete native input-to-command path, not a button-map inference or animation-name inference.
- No equivalent Resynced path was examined in this pass.

## Implication

A settings-only remap cannot be assumed to reproduce the recovered behavior: the native path considers edge/context state and actor/target candidates before issuing a typed command. A restoration mod will likely need a legitimate Resynced-side candidate/command interface, or a verified behavior-level seam with complete eligibility, cancellation, and pass-through signals. Neither integration seam is verified, so this is a feasibility direction, not hook guidance.

## Next discriminating step

Continue static cross-reference work to identify semantic names for the candidate discriminators and the callback's actor ownership. Then inspect Resynced resources/code through a safe, named single-entry route. If static labels remain ambiguous, use only an authorized witnessed run after the current no-launch restriction is lifted; capture input edge, candidate context, and submitted command together.

## Follow-up: ownership and routing boundary

A closer read of the behavior factory shows that the BhvAssassin initializer is selected through a generic behavior-type dispatch table. The callback registration proves that this behavior implementation exists and has an input-facing callback; it does not show which actor templates instantiate it. The traced path contains no verified Edward/player identity test. Ownership therefore remains **unknown** (player, NPC, or shared); authored component data would be needed to decide.

The candidate resolver has a local, confirmed fallback order: candidate resolver A runs first; resolver B runs only if A reports no candidate; resolver C runs only if B also reports none. A separate actor/target candidate pass runs afterward regardless. The first resolver checks candidate handles and eligibility predicates and only scans candidates when the compared runtime object is within a configured distance threshold. This is a local resolver ordering, not a global combat-action priority.

The typed router branches on operation categories and forwards handle-based operations into a generic message queue. Static inspection reached queue insertion but did not identify the consuming subscriber or an action-start function for this specific message. The numeric candidate and operation categories therefore remain **unlabeled**; no Attack, Finisher, Counter, Assassination, or named interaction can be assigned from this evidence.

**Result:** the path is confirmed from logical edge through ordered candidate checks to typed message submission. The remaining semantic boundary is the actor-template attachment and message recipient/action implementation. The best next static step is to inspect one controlled-player actor template and one comparable NPC template with a supported single-resource parser, then resolve the candidate event handles. No game was launched.
