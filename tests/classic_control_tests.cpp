#include "acbf/classic_control.h"

#include <iostream>
#include <string>

using namespace acbf::classic;

namespace {
int failures = 0;
int checks = 0;

void check(bool condition, const char* name) {
    ++checks;
    if (!condition) {
        ++failures;
        std::cerr << "FAIL: " << name << '\n';
    }
}

void check_result(Decision result, Status s, Intent a, const char* name) {
    check(result.status == s && result.intent == a, name);
}

Snapshot combat(Availability attack, Availability takedown) {
    Snapshot c{};
    c.zone = Zone::Combat;
    c.can_attack = attack;
    c.can_takedown = takedown;
    return c;
}

Snapshot stealth(Availability attack, Availability assassinate) {
    Snapshot c{};
    c.zone = Zone::Stealth;
    c.can_attack = attack;
    c.can_assassinate = assassinate;
    return c;
}
}  // namespace

int main() {
    // Press only once per physical down/up cycle, even with many held samples.
    EdgeTracker tracker;
    check(tracker.observe(Button::X, false) == Edge::Idle, "initial idle");
    check(tracker.observe(Button::X, true) == Edge::Pressed, "first press");
    for (int i = 0; i < 100; ++i)
        check(tracker.observe(Button::X, true) == Edge::Held, "no auto repeat");
    check(tracker.observe(Button::B, true) == Edge::Pressed, "independent B press");
    check(tracker.observe(Button::X, false) == Edge::Released, "release");
    check(tracker.observe(Button::X, false) == Edge::Idle, "idle after release");
    check(tracker.observe(Button::X, true) == Edge::Pressed, "re-press");
    tracker.reset();
    check(tracker.observe(Button::X, true) == Edge::Pressed, "reset clears held state");
    check(tracker.observe(Button::Other, true) == Edge::Idle, "unknown physical button ignored");

    // X-01/X-02. Contextual finisher is the candidate preferred action.
    check_result(resolve(Button::X, Edge::Pressed, combat(Availability::Yes, Availability::No)),
                 Status::Selected, Intent::Attack, "X-01: attack with no takedown");
    check_result(resolve(Button::X, Edge::Pressed, combat(Availability::Yes, Availability::Yes)),
                 Status::Selected, Intent::Takedown, "X-02: takedown preferred to attack");

    // X-03. Assassination eligibility is only meaningful in a verified mode.
    check_result(resolve(Button::X, Edge::Pressed, stealth(Availability::Yes, Availability::Yes)),
                 Status::Selected, Intent::Assassinate, "X-03: assassination preferred");
    check_result(resolve(Button::X, Edge::Pressed, stealth(Availability::Yes, Availability::No)),
                 Status::Selected, Intent::Attack, "X-03: normal attack fallback");

    // X-04/X-05. Never assume an Unknown eligibility is No.
    check_result(resolve(Button::X, Edge::Pressed, combat(Availability::Yes, Availability::Unknown)),
                 Status::Unresolved, Intent::None, "X-04: unknown finisher blocks speculation");
    check_result(resolve(Button::X, Edge::Pressed, combat(Availability::No, Availability::No)),
                 Status::NoAction, Intent::None, "X-04: no eligible attack");
    check_result(resolve(Button::X, Edge::Pressed, stealth(Availability::Unknown, Availability::No)),
                 Status::Unresolved, Intent::None, "X-05: unknown normal attack");
    check_result(resolve(Button::X, Edge::Held, combat(Availability::Yes, Availability::Yes)),
                 Status::Ignored, Intent::None, "X-05: held press not dispatched");
    check_result(resolve(Button::X, Edge::Released, combat(Availability::Yes, Availability::Yes)),
                 Status::Ignored, Intent::None, "X-05: release not dispatched");
    check_result(resolve(Button::X, Edge::Pressed, Snapshot{}),
                 Status::Unresolved, Intent::None, "Unknown game mode is unresolved");

    // Combat B defense. These rules are provisional; the game is not integrated.
    Snapshot c{};
    c.zone = Zone::Combat;
    c.can_counter = Availability::Yes;
    c.can_dodge = Availability::Yes;
    check_result(resolve(Button::B, Edge::Pressed, c),
                 Status::Selected, Intent::Counter, "B-01: prefer available counter");
    c.can_counter = Availability::No;
    check_result(resolve(Button::B, Edge::Pressed, c),
                 Status::Selected, Intent::Dodge, "B-02: dodge when counter unavailable");
    c.can_counter = Availability::Unknown;
    check_result(resolve(Button::B, Edge::Pressed, c),
                 Status::Unresolved, Intent::None, "B: never guess counter window");
    c.can_counter = Availability::No;
    c.can_dodge = Availability::Unknown;
    check_result(resolve(Button::B, Edge::Pressed, c),
                 Status::Unresolved, Intent::None, "B: never guess dodge window");
    c.can_dodge = Availability::No;
    check_result(resolve(Button::B, Edge::Pressed, c),
                 Status::NoAction, Intent::None, "B: no eligible defense");

    // A maps to a candidate guard-break intent only in combat.
    c.can_break_defense = Availability::Yes;
    check_result(resolve(Button::A, Edge::Pressed, c),
                 Status::Selected, Intent::BreakDefense, "A-01: candidate guard break");
    c.can_break_defense = Availability::Unknown;
    check_result(resolve(Button::A, Edge::Pressed, c),
                 Status::Unresolved, Intent::None, "A-01: unknown eligibility");
    c.can_break_defense = Availability::No;
    check_result(resolve(Button::A, Edge::Pressed, c),
                 Status::NoAction, Intent::None, "A-01: no eligible guard break");

    // Native contexts / not-yet-implemented RT never get an invented action.
    for (Zone zone : {Zone::Naval, Zone::Swimming, Zone::Menu}) {
        Snapshot n{};
        n.zone = zone;
        check_result(resolve(Button::X, Edge::Pressed, n),
                     Status::PassThrough, Intent::None, "naval/swimming/menu X untouched");
        check_result(resolve(Button::B, Edge::Pressed, n),
                     Status::PassThrough, Intent::None, "naval/swimming/menu B untouched");
        check_result(resolve(Button::A, Edge::Pressed, n),
                     Status::PassThrough, Intent::None, "naval/swimming/menu A untouched");
    }
    Snapshot t{};
    t.zone = Zone::Traversal;
    t.can_attack = Availability::No;
    check_result(resolve(Button::X, Edge::Pressed, t),
                 Status::NoAction, Intent::None, "traversal X no target");
    check_result(resolve(Button::B, Edge::Pressed, t),
                 Status::PassThrough, Intent::None, "traversal B stays native");
    check_result(resolve(Button::A, Edge::Pressed, t),
                 Status::PassThrough, Intent::None, "traversal A stays native");
    check_result(resolve(Button::RT, Edge::Pressed, t),
                 Status::PassThrough, Intent::None, "RT high profile not implemented");
    check_result(resolve(Button::Other, Edge::Pressed, t),
                 Status::PassThrough, Intent::None, "other input unaffected");

    std::cout << checks << " offline control-core assertions, "
              << failures << " failures\n";
    return failures == 0 ? 0 : 1;
}
