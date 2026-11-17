#pragma once

namespace acbf::legacy {

// Offline model of the documented original AC4 combat input phases.
// This is not an adapter for Resynced and must not consume live game input.
enum class Phase { Unknown, NeutralCombat, IncomingMelee, IncomingGunfire, CounterFollowUp };
enum class Button { X, Y, A, B, Other };
enum class Gesture { Tap, Hold, Other };
enum class Enemy { Unknown, Soldier, Scout, Gunner, Brute, Captain };
enum class Availability { Unknown, No, Yes };
enum class Action {
    None,
    NativePassThrough,
    Unresolved,
    Guard,
    EnterCounterFollowUp,
    DoubleCounter,
    EvadeBrute,
    CounterKill,
    Throw,
    DisarmOrStun,
    UseSelectedTool,
    BreakDefense,
    HumanShield
};

struct Input {
    Phase phase = Phase::Unknown;
    Button button = Button::Other;
    Gesture gesture = Gesture::Other;
    Enemy enemy = Enemy::Unknown;
    unsigned simultaneous_melee_warnings = 0;
    Availability nearby_shield_target = Availability::Unknown;
};

struct Decision {
    Action action;
};

// Selects a documented candidate action from an already-known combat phase.
// Timing, phase detection, input ownership and native dispatch are intentionally
// outside this pure model and remain unverified.
Decision resolve(const Input& input) noexcept;

}  // namespace acbf::legacy
