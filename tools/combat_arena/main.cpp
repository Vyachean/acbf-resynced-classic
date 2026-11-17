#include "acbf/legacy_combat_model.h"

#include <charconv>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

namespace {
using namespace acbf::legacy;

const char* name(Phase value) noexcept {
    switch (value) {
    case Phase::Unknown: return "unknown";
    case Phase::NeutralCombat: return "neutral-combat";
    case Phase::IncomingMelee: return "incoming-melee";
    case Phase::IncomingGunfire: return "incoming-gunfire";
    case Phase::CounterFollowUp: return "counter-follow-up";
    }
    return "unknown";
}

const char* name(Enemy value) noexcept {
    switch (value) {
    case Enemy::Unknown: return "unknown";
    case Enemy::Soldier: return "soldier";
    case Enemy::Scout: return "scout";
    case Enemy::Gunner: return "gunner";
    case Enemy::Brute: return "brute";
    case Enemy::Captain: return "captain";
    }
    return "unknown";
}

const char* name(Action value) noexcept {
    switch (value) {
    case Action::None: return "none";
    case Action::NativePassThrough: return "native-pass-through";
    case Action::Unresolved: return "unresolved";
    case Action::Guard: return "guard";
    case Action::EnterCounterFollowUp: return "enter-counter-follow-up";
    case Action::DoubleCounter: return "double-counter";
    case Action::EvadeBrute: return "evade-brute";
    case Action::CounterKill: return "counter-kill";
    case Action::Throw: return "throw";
    case Action::DisarmOrStun: return "disarm-or-stun";
    case Action::UseSelectedTool: return "use-selected-tool";
    case Action::BreakDefense: return "break-defense";
    case Action::HumanShield: return "human-shield";
    }
    return "unknown";
}

bool parse_button(std::string_view value, Button& result) noexcept {
    if (value == "x") result = Button::X;
    else if (value == "y") result = Button::Y;
    else if (value == "a") result = Button::A;
    else if (value == "b") result = Button::B;
    else if (value == "other") result = Button::Other;
    else return false;
    return true;
}

bool parse_phase(std::string_view value, Phase& result) noexcept {
    if (value == "unknown") result = Phase::Unknown;
    else if (value == "neutral") result = Phase::NeutralCombat;
    else if (value == "melee") result = Phase::IncomingMelee;
    else if (value == "gunfire") result = Phase::IncomingGunfire;
    else if (value == "followup") result = Phase::CounterFollowUp;
    else return false;
    return true;
}

bool parse_enemy(std::string_view value, Enemy& result) noexcept {
    if (value == "unknown") result = Enemy::Unknown;
    else if (value == "soldier") result = Enemy::Soldier;
    else if (value == "scout") result = Enemy::Scout;
    else if (value == "gunner") result = Enemy::Gunner;
    else if (value == "brute") result = Enemy::Brute;
    else if (value == "captain") result = Enemy::Captain;
    else return false;
    return true;
}

bool parse_availability(std::string_view value, Availability& result) noexcept {
    if (value == "unknown") result = Availability::Unknown;
    else if (value == "yes") result = Availability::Yes;
    else if (value == "no") result = Availability::No;
    else return false;
    return true;
}

void print_state(const Input& state) {
    std::cout << "phase=" << name(state.phase)
              << " enemy=" << name(state.enemy)
              << " warnings=" << state.simultaneous_melee_warnings
              << " shield-target="
              << (state.nearby_shield_target == Availability::Yes ? "yes" :
                  state.nearby_shield_target == Availability::No ? "no" : "unknown")
              << '\n';
}

void print_help() {
    std::cout << "Commands:\n"
              << "  set phase neutral|melee|gunfire|followup|unknown\n"
              << "  set enemy soldier|scout|gunner|brute|captain|unknown\n"
              << "  set warnings <count>\n"
              << "  set shield yes|no|unknown\n"
              << "  press x|b|a|y|other tap|hold\n"
              << "  status | reset | help | quit\n"
              << "A B tap counter moves to follow-up; choose X/B/A/Y there.\n";
}

bool set_after_action(Input& state, Action action) noexcept {
    if (action == Action::EnterCounterFollowUp) {
        state.phase = Phase::CounterFollowUp;
        return true;
    }
    if (state.phase == Phase::CounterFollowUp &&
        action != Action::Unresolved && action != Action::NativePassThrough) {
        state.phase = Phase::NeutralCombat;
        return true;
    }
    return false;
}

bool run_demo() {
    Input state{};
    state.phase = Phase::IncomingMelee;
    state.enemy = Enemy::Soldier;
    state.simultaneous_melee_warnings = 1;
    const auto counter = resolve({state.phase, Button::B, Gesture::Tap,
                                  state.enemy, state.simultaneous_melee_warnings,
                                  state.nearby_shield_target}).action;
    std::cout << "incoming soldier + B tap -> " << name(counter) << '\n';
    if (counter != Action::EnterCounterFollowUp) return false;
    (void)set_after_action(state, counter);

    const auto kill = resolve({state.phase, Button::X, Gesture::Tap, state.enemy,
                               state.simultaneous_melee_warnings,
                               state.nearby_shield_target}).action;
    std::cout << "counter follow-up + X tap -> " << name(kill) << '\n';
    if (kill != Action::CounterKill) return false;
    (void)set_after_action(state, kill);

    const auto guard = resolve({Phase::NeutralCombat, Button::B, Gesture::Hold,
                                Enemy::Soldier, 0, Availability::No}).action;
    std::cout << "neutral combat + B hold -> " << name(guard) << '\n';
    if (guard != Action::Guard) return false;

    const auto brute = resolve({Phase::IncomingMelee, Button::B, Gesture::Tap,
                                Enemy::Brute, 1, Availability::No}).action;
    std::cout << "brute warning + B tap -> " << name(brute) << '\n';
    if (brute != Action::EvadeBrute) return false;

    const auto double_counter = resolve({Phase::IncomingMelee, Button::B,
                                         Gesture::Tap, Enemy::Soldier, 2,
                                         Availability::No}).action;
    std::cout << "two simultaneous warnings + B tap -> "
              << name(double_counter) << '\n';
    if (double_counter != Action::DoubleCounter) return false;

    const auto shield = resolve({Phase::IncomingGunfire, Button::A, Gesture::Tap,
                                 Enemy::Gunner, 0, Availability::Yes}).action;
    std::cout << "gunfire + A tap + nearby civilian -> " << name(shield) << '\n';
    if (shield != Action::HumanShield) return false;

    const auto unknown = resolve({Phase::Unknown, Button::B, Gesture::Tap,
                                  Enemy::Unknown, 0, Availability::Unknown}).action;
    std::cout << "unknown phase + B tap -> " << name(unknown) << '\n';
    if (unknown != Action::Unresolved) return false;

    std::cout << "offline combat arena demo passed\n";
    return true;
}

}  // namespace

int main(int argc, char** argv) {
    if (argc == 2 && std::string_view(argv[1]) == "--demo")
        return run_demo() ? 0 : 1;
    if (argc != 1) {
        std::cerr << "usage: acbf_combat_arena [--demo]\n";
        return 2;
    }

    Input state{};
    state.phase = Phase::NeutralCombat;
    state.enemy = Enemy::Soldier;
    state.simultaneous_melee_warnings = 1;
    state.nearby_shield_target = Availability::No;

    std::cout << "AC4 policy sandbox (offline model only; no game controls).\n";
    print_help();
    print_state(state);

    std::string line;
    while (std::cout << "> " && std::getline(std::cin, line)) {
        std::istringstream input(line);
        std::string command;
        std::string first;
        std::string second;
        input >> command >> first >> second;

        if (command == "quit" || command == "q") return 0;
        if (command == "help" || command == "h") {
            print_help();
            continue;
        }
        if (command == "status") {
            print_state(state);
            continue;
        }
        if (command == "reset") {
            state = {};
            state.phase = Phase::NeutralCombat;
            state.enemy = Enemy::Soldier;
            state.simultaneous_melee_warnings = 1;
            state.nearby_shield_target = Availability::No;
            print_state(state);
            continue;
        }
        if (command == "set" && first == "phase") {
            Phase value{};
            if (!parse_phase(second, value)) {
                std::cout << "unknown phase; use help\n";
                continue;
            }
            state.phase = value;
            print_state(state);
            continue;
        }
        if (command == "set" && first == "enemy") {
            Enemy value{};
            if (!parse_enemy(second, value)) {
                std::cout << "unknown enemy; use help\n";
                continue;
            }
            state.enemy = value;
            print_state(state);
            continue;
        }
        if (command == "set" && first == "warnings") {
            unsigned value = 0;
            const auto result = std::from_chars(second.data(),
                                                second.data() + second.size(), value);
            if (result.ec != std::errc{} || result.ptr != second.data() + second.size()) {
                std::cout << "warnings must be a non-negative integer\n";
                continue;
            }
            state.simultaneous_melee_warnings = value;
            print_state(state);
            continue;
        }
        if (command == "set" && first == "shield") {
            Availability value{};
            if (!parse_availability(second, value)) {
                std::cout << "shield target must be yes|no|unknown\n";
                continue;
            }
            state.nearby_shield_target = value;
            print_state(state);
            continue;
        }
        if (command == "press") {
            Button button{};
            if (!parse_button(first, button)) {
                std::cout << "unknown button; use help\n";
                continue;
            }
            Gesture gesture{};
            if (second == "tap") gesture = Gesture::Tap;
            else if (second == "hold") gesture = Gesture::Hold;
            else {
                std::cout << "gesture must be tap|hold\n";
                continue;
            }
            const Action action = resolve({state.phase, button, gesture, state.enemy,
                                           state.simultaneous_melee_warnings,
                                           state.nearby_shield_target}).action;
            std::cout << name(state.phase) << " + " << first << ' ' << second
                      << " -> " << name(action) << '\n';
            if (set_after_action(state, action)) print_state(state);
            continue;
        }
        std::cout << "unknown command; use help\n";
    }
    return 0;
}
