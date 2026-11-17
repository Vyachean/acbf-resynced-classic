#include "acbf/legacy_combat_model.h"

#include <cstdlib>
#include <iostream>

using namespace acbf::legacy;

namespace {
void expect(const char* name, Action actual, Action expected) {
    if (actual == expected) return;
    std::cerr << name << ": got " << static_cast<int>(actual)
              << ", expected " << static_cast<int>(expected) << '\n';
    std::exit(1);
}
}

int main() {
    Input input;
    expect("unknown state fails closed", resolve(input).action, Action::Unresolved);

    input.phase = Phase::NeutralCombat;
    input.button = Button::B;
    input.gesture = Gesture::Hold;
    expect("hold B guards", resolve(input).action, Action::Guard);
    input.gesture = Gesture::Tap;
    expect("tap B outside warning passes through", resolve(input).action,
           Action::NativePassThrough);

    input.phase = Phase::IncomingMelee;
    input.enemy = Enemy::Soldier;
    expect("tap B enters counter follow-up", resolve(input).action,
           Action::EnterCounterFollowUp);
    input.gesture = Gesture::Hold;
    expect("hold B remains guard", resolve(input).action, Action::Guard);
    input.gesture = Gesture::Tap;
    input.enemy = Enemy::Brute;
    expect("Brute counter input evades", resolve(input).action, Action::EvadeBrute);
    input.enemy = Enemy::Captain;
    expect("Captain counter routing remains unresolved", resolve(input).action,
           Action::Unresolved);
    input.enemy = Enemy::Soldier;
    input.simultaneous_melee_warnings = 2;
    expect("two warnings use double counter", resolve(input).action,
           Action::DoubleCounter);
    input.simultaneous_melee_warnings = 1;

    input.phase = Phase::CounterFollowUp;
    input.button = Button::X;
    expect("follow-up X kills", resolve(input).action, Action::CounterKill);
    input.button = Button::B;
    expect("follow-up B throws", resolve(input).action, Action::Throw);
    input.button = Button::A;
    expect("follow-up A disarms or stuns", resolve(input).action,
           Action::DisarmOrStun);
    input.button = Button::Y;
    expect("follow-up Y uses selected tool", resolve(input).action,
           Action::UseSelectedTool);
    input.gesture = Gesture::Hold;
    expect("held follow-up passes through", resolve(input).action,
           Action::NativePassThrough);

    input.phase = Phase::IncomingGunfire;
    input.button = Button::A;
    input.gesture = Gesture::Tap;
    input.nearby_shield_target = Availability::Unknown;
    expect("unknown shield target fails closed", resolve(input).action,
           Action::Unresolved);
    input.nearby_shield_target = Availability::Yes;
    expect("nearby target enables human shield", resolve(input).action,
           Action::HumanShield);
    input.nearby_shield_target = Availability::No;
    expect("no nearby target keeps native handling", resolve(input).action,
           Action::NativePassThrough);

    std::cout << "legacy combat model scenarios passed\n";
}
