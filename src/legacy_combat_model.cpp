#include "acbf/legacy_combat_model.h"

namespace acbf::legacy {
namespace {

Decision choose(Action action) noexcept { return {action}; }

Decision follow_up(Button button) noexcept {
    switch (button) {
    case Button::X: return choose(Action::CounterKill);
    case Button::B: return choose(Action::Throw);
    case Button::A: return choose(Action::DisarmOrStun);
    case Button::Y: return choose(Action::UseSelectedTool);
    case Button::Other: return choose(Action::NativePassThrough);
    }
    return choose(Action::Unresolved);
}

}  // namespace

Decision resolve(const Input& input) noexcept {
    if (input.phase == Phase::Unknown)
        return choose(Action::Unresolved);

    if (input.phase == Phase::CounterFollowUp)
        return input.gesture == Gesture::Tap
                   ? follow_up(input.button)
                   : choose(Action::NativePassThrough);

    if (input.phase == Phase::NeutralCombat) {
        if (input.button == Button::A && input.gesture == Gesture::Tap)
            return choose(Action::BreakDefense);
        if (input.button == Button::B && input.gesture == Gesture::Hold)
            return choose(Action::Guard);
        return choose(Action::NativePassThrough);
    }

    if (input.phase == Phase::IncomingMelee && input.button == Button::B) {
        if (input.gesture == Gesture::Hold)
            return choose(Action::Guard);
        if (input.gesture != Gesture::Tap)
            return choose(Action::NativePassThrough);
        if (input.simultaneous_melee_warnings > 1)
            return choose(Action::DoubleCounter);
        if (input.enemy == Enemy::Brute)
            return choose(Action::EvadeBrute);
        if (input.enemy == Enemy::Unknown || input.enemy == Enemy::Captain)
            return choose(Action::Unresolved);
        return choose(Action::EnterCounterFollowUp);
    }

    if (input.phase == Phase::IncomingGunfire && input.button == Button::A &&
        input.gesture == Gesture::Tap) {
        if (input.nearby_shield_target == Availability::Unknown)
            return choose(Action::Unresolved);
        if (input.nearby_shield_target == Availability::Yes)
            return choose(Action::HumanShield);
        return choose(Action::NativePassThrough);
    }

    return choose(Action::NativePassThrough);
}

}  // namespace acbf::legacy
