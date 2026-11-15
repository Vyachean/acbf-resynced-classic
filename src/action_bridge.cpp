#include "acbf/action_bridge.h"

namespace acbf::bridge {

Outcome XActionBridge::prepare(classic::Button button, classic::Edge edge,
                               const classic::Snapshot& snapshot,
                               const IntegrationEvidence& evidence,
                               const NativeActionSink& sink) noexcept {
    // No ability to install an input-consuming hook until ALL six gates are
    // demonstrated on the exact build. Unknown eligibility fails closed.
    if (button != classic::Button::X || edge != classic::Edge::Pressed ||
        !evidence.complete() || consumed_until_release_)
        return {};

    if (pending_ticket_ != 0)
        return {Result::AlreadyPending, classic::Intent::None, pending_ticket_};

    const classic::Decision decision = classic::resolve(button, edge, snapshot);
    if (decision.status != classic::Status::Selected ||
        decision.intent == classic::Intent::None ||
        !sink.supports(decision.intent))
        return {};

    // Ticket 0 is reserved for "no pending candidate"; avoid 0 on wrap.
    ++next_ticket_;
    if (next_ticket_ == 0) ++next_ticket_;
    pending_ticket_ = next_ticket_;
    pending_intent_ = decision.intent;
    return {Result::ReadyToClaim, pending_intent_, pending_ticket_};
}

Outcome XActionBridge::commit(std::uint64_t ticket, Claim claim,
                              NativeActionSink& sink) noexcept {
    if (ticket == 0 || ticket != pending_ticket_ || consumed_until_release_)
        return {Result::StaleTicket, classic::Intent::None, 0};

    const classic::Intent intent = pending_intent_;
    pending_ticket_ = 0;
    pending_intent_ = classic::Intent::None;

    if (claim != Claim::Claimed)
        return {Result::Passthrough, classic::Intent::None, 0};

    // The hook owns the entire physical X cycle before an action is invoked.
    // Even if the action bridge rejects or reports ambiguity, never replay X:
    // doing so might dispatch a *second* normal attack/takedown.
    consumed_until_release_ = true;
    const Delivery result = sink.submit(intent);
    switch (result) {
    case Delivery::Accepted:
        return {Result::ConsumedAccepted, intent, ticket};
    case Delivery::RejectedNoEffect:
        return {Result::ConsumedRejected, intent, ticket};
    case Delivery::Indeterminate:
        return {Result::ConsumedIndeterminate, intent, ticket};
    }
    return {Result::ConsumedIndeterminate, intent, ticket};
}

Outcome XActionBridge::continue_cycle(classic::Button button,
                                      classic::Edge edge) noexcept {
    if (button != classic::Button::X ||
        (edge != classic::Edge::Held && edge != classic::Edge::Released))
        return {};
    if (!consumed_until_release_) return {};

    if (edge == classic::Edge::Released)
        consumed_until_release_ = false;
    return {Result::ConsumedEdge, classic::Intent::None, 0};
}

bool XActionBridge::cancel_pending(std::uint64_t ticket) noexcept {
    if (ticket == 0 || ticket != pending_ticket_ ||
        consumed_until_release_) return false;
    pending_ticket_ = 0;
    pending_intent_ = classic::Intent::None;
    return true;
}

} // namespace acbf::bridge
