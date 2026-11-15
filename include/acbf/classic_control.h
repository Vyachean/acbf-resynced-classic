#pragma once

#include <array>
#include <cstddef>

namespace acbf::classic {

// These are abstract controller inputs and verified/unknown game-context signals.
// They are NOT native game event IDs, offsets, hooks, or an ACBFR SDK.
enum class Button { X, Y, B, A, LB, RB, LT, RT, Other };
enum class Edge { Pressed, Held, Released, Idle };
enum class Zone { Unknown, Combat, Stealth, Traversal, Naval, Swimming, Menu };
enum class Availability { Unknown, No, Yes };
enum class Intent { None, Attack, Takedown, Assassinate, Counter, Dodge, BreakDefense };
enum class Status { Selected, NoAction, PassThrough, Unresolved, Ignored };
enum class Reason {
    SelectedEligible,
    NoEligibleAction,
    UnknownEligibility,
    UnknownZone,
    NativeContext,
    NotImplemented,
    NotPressed
};

struct Snapshot {
    Zone zone = Zone::Unknown;
    Availability can_attack = Availability::Unknown;
    Availability can_takedown = Availability::Unknown;
    Availability can_assassinate = Availability::Unknown;
    Availability can_counter = Availability::Unknown;
    Availability can_dodge = Availability::Unknown;
    Availability can_break_defense = Availability::Unknown;
};

struct Decision {
    Status status;
    Intent intent;
    Reason reason;
};

// The resolution result is a *candidate* intent, not a command sent to a game.
// Integration must never treat Status::Unresolved as a safe choice to hook/consume
// an input. It must disable that override or follow an explicitly safe fallback.
Decision resolve(Button button, Edge edge, const Snapshot& context) noexcept;

// Transforms one sampled button state into one Pressed edge, held samples,
// one Released edge and subsequent idle samples. No OS/game dependencies.
class EdgeTracker {
public:
    Edge observe(Button button, bool pressed) noexcept;
    void reset() noexcept;

private:
    std::array<bool, 8> down_{};
};

}  // namespace acbf::classic
