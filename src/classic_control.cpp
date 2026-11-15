#include "acbf/classic_control.h"

namespace acbf::classic {
namespace {

constexpr Decision selected(Intent intent) noexcept {
    return {Status::Selected, intent, Reason::SelectedEligible};
}

constexpr Decision unresolved(Reason reason) noexcept {
    return {Status::Unresolved, Intent::None, reason};
}

constexpr Decision no_action() noexcept {
    return {Status::NoAction, Intent::None, Reason::NoEligibleAction};
}

constexpr Decision pass_through(Reason reason) noexcept {
    return {Status::PassThrough, Intent::None, reason};
}

Decision resolve_x(const Snapshot& c) noexcept {
    switch (c.zone) {
    case Zone::Combat:
        // *Proposed* classic preference, not a recovered Resynced priority.
        if (c.can_takedown == Availability::Unknown)
            return unresolved(Reason::UnknownEligibility);
        if (c.can_takedown == Availability::Yes)
            return selected(Intent::Takedown);
        if (c.can_attack == Availability::Unknown)
            return unresolved(Reason::UnknownEligibility);
        return c.can_attack == Availability::Yes ? selected(Intent::Attack) : no_action();

    case Zone::Stealth:
        if (c.can_assassinate == Availability::Unknown)
            return unresolved(Reason::UnknownEligibility);
        if (c.can_assassinate == Availability::Yes)
            return selected(Intent::Assassinate);
        if (c.can_attack == Availability::Unknown)
            return unresolved(Reason::UnknownEligibility);
        return c.can_attack == Availability::Yes ? selected(Intent::Attack) : no_action();

    case Zone::Traversal:
        if (c.can_attack == Availability::Unknown)
            return unresolved(Reason::UnknownEligibility);
        return c.can_attack == Availability::Yes ? selected(Intent::Attack) : no_action();

    case Zone::Naval:
    case Zone::Swimming:
    case Zone::Menu:
        return pass_through(Reason::NativeContext);

    case Zone::Unknown:
        return unresolved(Reason::UnknownZone);
    }
    return unresolved(Reason::UnknownZone);
}

Decision resolve_b(const Snapshot& c) noexcept {
    if (c.zone == Zone::Unknown)
        return unresolved(Reason::UnknownZone);

    if (c.zone != Zone::Combat)
        return pass_through(Reason::NativeContext);

    // Proposed policy: prefer a valid counter; otherwise select a valid dodge.
    // If a decision-relevant availability is missing, do not guess.
    if (c.can_counter == Availability::Unknown)
        return unresolved(Reason::UnknownEligibility);
    if (c.can_counter == Availability::Yes)
        return selected(Intent::Counter);

    if (c.can_dodge == Availability::Unknown)
        return unresolved(Reason::UnknownEligibility);
    return c.can_dodge == Availability::Yes ? selected(Intent::Dodge) : no_action();
}

Decision resolve_a(const Snapshot& c) noexcept {
    if (c.zone == Zone::Unknown)
        return unresolved(Reason::UnknownZone);

    if (c.zone != Zone::Combat)
        return pass_through(Reason::NativeContext);

    if (c.can_break_defense == Availability::Unknown)
        return unresolved(Reason::UnknownEligibility);
    return c.can_break_defense == Availability::Yes
               ? selected(Intent::BreakDefense)
               : no_action();
}

}  // namespace

Decision resolve(Button button, Edge edge, const Snapshot& context) noexcept {
    // Downstream integration must pass release transitions to any native
    // input route it consumes. This function is *not* a native input adapter.
    if (edge != Edge::Pressed)
        return {Status::Ignored, Intent::None, Reason::NotPressed};

    switch (button) {
    case Button::X:
        return resolve_x(context);
    case Button::B:
        return resolve_b(context);
    case Button::A:
        return resolve_a(context);
    case Button::RT:
        // High-profile / RT action semantics are deliberately unimplemented.
        return pass_through(Reason::NotImplemented);
    case Button::Y:
    case Button::LB:
    case Button::RB:
    case Button::LT:
    case Button::Other:
        return pass_through(Reason::NotImplemented);
    }
    return pass_through(Reason::NotImplemented);
}

Edge EdgeTracker::observe(Button button, bool pressed) noexcept {
    std::size_t index = 0;
    switch (button) {
    case Button::X: index = 0; break;
    case Button::Y: index = 1; break;
    case Button::B: index = 2; break;
    case Button::A: index = 3; break;
    case Button::LB: index = 4; break;
    case Button::RB: index = 5; break;
    case Button::LT: index = 6; break;
    case Button::RT: index = 7; break;
    case Button::Other: return Edge::Idle;
    }
    const bool previous = down_[index];
    down_[index] = pressed;
    if (pressed && !previous) return Edge::Pressed;
    if (pressed && previous) return Edge::Held;
    if (!pressed && previous) return Edge::Released;
    return Edge::Idle;
}

void EdgeTracker::reset() noexcept {
    down_.fill(false);
}

}  // namespace acbf::classic
