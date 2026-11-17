#include "acbf/action_bridge.h"

#include <iostream>

using namespace acbf::bridge;
using namespace acbf::classic;

namespace {
unsigned failures = 0;
unsigned checks = 0;

void check(bool condition, const char* label) {
    ++checks;
    if (!condition) {
        ++failures;
        std::cerr << "FAIL: " << label << '\n';
    }
}
void result(const Outcome& output, Result expected, const char* name) {
    check(output.result == expected, name);
}

class FakeNative final : public NativeActionSink {
public:
    bool capable = true;
    Delivery delivery = Delivery::Accepted;
    unsigned submit_count = 0;
    Intent last = Intent::None;

    bool supports(Intent intent) const noexcept override {
        return capable && intent != Intent::None;
    }
    Delivery submit(Intent intent) noexcept override {
        ++submit_count;
        last = intent;
        return delivery;
    }
};

IntegrationEvidence all_gates() {
    return {true, true, true, true, true, true, true, true, true, true};
}
Snapshot combat(Availability attack, Availability takedown) {
    Snapshot snapshot{};
    snapshot.zone = Zone::Combat;
    snapshot.can_attack = attack;
    snapshot.can_takedown = takedown;
    return snapshot;
}
Snapshot stealth(Availability attack, Availability assassinate) {
    Snapshot snapshot{};
    snapshot.zone = Zone::Stealth;
    snapshot.can_attack = attack;
    snapshot.can_assassinate = assassinate;
    return snapshot;
}
}

int main() {
    FakeNative sink;
    XActionBridge pipeline;
    const Snapshot finisher = combat(Availability::Yes, Availability::Yes);
    const IntegrationEvidence verified = all_gates();
    const auto try_x = [&](const Snapshot& c, const IntegrationEvidence& e) {
        return pipeline.prepare(Button::X, Edge::Pressed, c, e, sink);
    };

    // Each feature-scoped safety gate is individually necessary.
    result(try_x(finisher, {}), Result::Passthrough, "default closed");
    for (int mask = 0; mask < 10; ++mask) {
        IntegrationEvidence proof = verified;
        switch (mask) {
        case 0: proof.exact_executable_verified = false; break;
        case 1: proof.input_interception_verified = false; break;
        case 2: proof.original_input_suppression_verified = false; break;
        case 3: proof.input_release_handling_verified = false; break;
        case 4: proof.context_eligibility_verified = false; break;
        case 5: proof.native_action_dispatch_verified = false; break;
        case 6: proof.affected_actions_reviewed = false; break;
        case 7: proof.unsupported_contexts_passthrough = false; break;
        case 8: proof.rollback_verified = false; break;
        case 9: proof.off_by_default_verified = false; break;
        }
        result(try_x(finisher, proof), Result::Passthrough,
               "single missing gate blocks claim and dispatch");
    }
    check(sink.submit_count == 0, "no native calls when gates unverified");
    result(pipeline.continue_cycle(Button::X, Edge::Released),
           Result::Passthrough, "no false consumed release");

    // No guessed game state, even when all integration gates are verified.
    result(try_x(Snapshot{}, verified), Result::Passthrough, "unknown zone");
    result(try_x(combat(Availability::Yes, Availability::Unknown), verified),
           Result::Passthrough, "unknown takedown availability");
    result(try_x(combat(Availability::Unknown, Availability::No), verified),
           Result::Passthrough, "unknown attack availability");
    result(try_x(combat(Availability::No, Availability::No), verified),
           Result::Passthrough, "no selected action");
    sink.capable = false;
    result(try_x(finisher, verified), Result::Passthrough,
           "native handler unsupported");
    sink.capable = true;

    // Native contexts and non-X inputs cannot become claimed by mistake.
    Snapshot menu{};
    menu.zone = Zone::Menu;
    result(try_x(menu, verified), Result::Passthrough, "menu native");
    result(pipeline.prepare(Button::B, Edge::Pressed, finisher, verified, sink),
           Result::Passthrough, "B remains native");
    result(pipeline.prepare(Button::Y, Edge::Pressed, finisher, verified, sink),
           Result::Passthrough, "Y remains native");
    result(pipeline.prepare(Button::X, Edge::Held, finisher, verified, sink),
           Result::Passthrough, "held sample cannot claim");
    result(pipeline.prepare(Button::X, Edge::Released, finisher, verified, sink),
           Result::Passthrough, "release cannot claim");

    // The plan alone is NOT a native command or input suppression.
    Outcome plan = try_x(finisher, verified);
    result(plan, Result::ReadyToClaim, "finisher candidate");
    check(plan.ticket != 0 && plan.intent == Intent::Takedown,
          "prepared takedown ticket");
    result(try_x(finisher, verified), Result::AlreadyPending,
           "no duplicate outstanding proposal");
    check(sink.submit_count == 0, "proposal is inert");
    result(pipeline.commit(plan.ticket + 1, Claim::Claimed, sink),
           Result::StaleTicket, "invalid ticket rejected");
    check(sink.submit_count == 0, "stale ticket did not call game");
    result(pipeline.commit(plan.ticket, Claim::NotClaimed, sink),
           Result::Passthrough, "unclaimed input cannot dispatch");
    check(sink.submit_count == 0, "no submission without ownership");
    result(pipeline.commit(plan.ticket, Claim::Claimed, sink),
           Result::StaleTicket, "unclaimed proposal invalidated");

    // Successful transaction: EXACTLY ONE action, entire physical cycle owned.
    plan = try_x(finisher, verified);
    result(pipeline.commit(plan.ticket, Claim::Claimed, sink),
           Result::ConsumedAccepted, "claimed and accepted");
    check(sink.submit_count == 1 && sink.last == Intent::Takedown,
          "single native takedown submitted");
    result(pipeline.commit(plan.ticket, Claim::Claimed, sink),
           Result::StaleTicket, "double commit rejected");
    for (int i = 0; i < 250; ++i)
        result(pipeline.continue_cycle(Button::X, Edge::Held),
               Result::ConsumedEdge, "held X remains owned, never resubmits");
    result(try_x(finisher, verified), Result::Passthrough,
           "no new press claim until release");
    check(sink.submit_count == 1, "held/extra press never resubmits");
    result(pipeline.continue_cycle(Button::B, Edge::Released),
           Result::Passthrough, "B release unaffected");
    result(pipeline.continue_cycle(Button::X, Edge::Released),
           Result::ConsumedEdge, "X release consumed");
    result(pipeline.continue_cycle(Button::X, Edge::Released),
           Result::Passthrough, "second release not consumed");

    // Rejected or ambiguous game calls may not trigger native X fallback,
    // because the hook already claimed the press and a retry can double fire.
    for (Delivery delivery : {Delivery::RejectedNoEffect, Delivery::Indeterminate}) {
        sink.delivery = delivery;
        plan = try_x(combat(Availability::Yes, Availability::No), verified);
        check(plan.intent == Intent::Attack, "attack fallback priority");
        const unsigned before = sink.submit_count;
        const auto outcome = pipeline.commit(plan.ticket, Claim::Claimed, sink);
        result(outcome, delivery == Delivery::RejectedNoEffect
                        ? Result::ConsumedRejected : Result::ConsumedIndeterminate,
               "failed native dispatch stays owned");
        check(sink.submit_count == before + 1, "one attempted action, no retries");
        result(pipeline.continue_cycle(Button::X, Edge::Released),
               Result::ConsumedEdge, "failed cycle release consumed");
        check(sink.submit_count == before + 1, "no replay after failure");
    }

    // Cancel does not call native actions, permits fresh planning.
    const unsigned before = sink.submit_count;
    plan = try_x(finisher, verified);
    check(pipeline.cancel_pending(plan.ticket), "cancel pending");
    check(!pipeline.cancel_pending(plan.ticket), "cancel cannot repeat");
    result(pipeline.commit(plan.ticket, Claim::Claimed, sink),
           Result::StaleTicket, "cancelled plan cannot dispatch");
    check(sink.submit_count == before, "cancel no game call");

    // Stealth preference is expressed only with synthetic verified eligibility.
    sink.delivery = Delivery::Accepted;
    plan = try_x(stealth(Availability::Yes, Availability::Yes), verified);
    check(plan.intent == Intent::Assassinate, "stealth intended assassination");
    result(pipeline.commit(plan.ticket, Claim::Claimed, sink),
           Result::ConsumedAccepted, "stealth submitted via generic seam");
    check(sink.last == Intent::Assassinate, "native adapter received intent");
    result(pipeline.continue_cycle(Button::X, Edge::Released),
           Result::ConsumedEdge, "stealth release consumed");

    std::cout << checks << " offline action bridge assertions, "
              << failures << " failures\n";
    return failures ? 1 : 0;
}
