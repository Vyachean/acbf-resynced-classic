#pragma once

#include "acbf/classic_control.h"

#include <cstdint>

namespace acbf::bridge {

// This is an OFFLINE integration contract, not a Resynced SDK or game hook.
// A valid fingerprint by itself cannot authorize execution of a game action.
// Every gate defaults to false and must be demonstrated independently.
struct IntegrationEvidence {
    bool exact_executable_verified = false;
    bool input_interception_verified = false;
    bool original_input_suppression_verified = false;
    bool input_release_handling_verified = false;
    bool context_eligibility_verified = false;
    bool native_action_dispatch_verified = false;

    bool complete() const noexcept {
        return exact_executable_verified &&
               input_interception_verified &&
               original_input_suppression_verified &&
               input_release_handling_verified &&
               context_eligibility_verified &&
               native_action_dispatch_verified;
    }
};

// The caller must first atomically claim the original physical X press
// (and its hold/release transitions) BEFORE invoking commit(). The claim is
// specific to ONE physical down/up cycle. No game input interception exists
// in this project today.
enum class Claim { NotClaimed, Claimed };

enum class Delivery {
    Accepted,            // native bridge specifically acknowledges submission
    RejectedNoEffect,    // native bridge certifies no game action occurred
    Indeterminate       // cannot safely retry or fall back
};

class NativeActionSink {
public:
    virtual ~NativeActionSink() = default;
    virtual bool supports(classic::Intent intent) const noexcept = 0;
    virtual Delivery submit(classic::Intent intent) noexcept = 0;
};

enum class Result {
    Passthrough,          // no input was claimed; native input remains untouched
    ReadyToClaim,         // candidate only; no submission or suppression yet
    AlreadyPending,       // repeated prepare; no second candidate
    StaleTicket,          // no submission; invalid or duplicate commit
    ConsumedAccepted,     // claim succeeded and action was submitted exactly once
    ConsumedRejected,     // claim succeeded; no effect, never replay input
    ConsumedIndeterminate,// claim succeeded; side effects unknown, never replay
    ConsumedEdge          // held/release of a claimed input cycle
};

struct Outcome {
    Result result = Result::Passthrough;
    classic::Intent intent = classic::Intent::None;
    std::uint64_t ticket = 0;
};

class XActionBridge {
public:
    // X ONLY: no speculative B/A/RT dispatch. EdgeTracker remains separate.
    Outcome prepare(classic::Button button, classic::Edge edge,
                    const classic::Snapshot& snapshot,
                    const IntegrationEvidence& evidence,
                    const NativeActionSink& sink) noexcept;

    // Submit ONLY after the game-side input hook confirms the physical X
    // press was claimed and the matching hold/release will also be handled.
    // No claim -> no game call. Failure after claim NEVER replays the press.
    Outcome commit(std::uint64_t ticket, Claim claim,
                   NativeActionSink& sink) noexcept;

    // Called for a held/released X sample in the same physical cycle.
    // Release clears only the consumed state, not a submitted action.
    Outcome continue_cycle(classic::Button button,
                           classic::Edge edge) noexcept;

    // Caller may cancel an UNCLAIMED candidate. Claimed cycles must be
    // observed through release to avoid silently dropping ownership.
    bool cancel_pending(std::uint64_t ticket) noexcept;

private:
    std::uint64_t next_ticket_ = 0;
    std::uint64_t pending_ticket_ = 0;
    classic::Intent pending_intent_ = classic::Intent::None;
    bool consumed_until_release_ = false;
};

}  // namespace acbf::bridge
