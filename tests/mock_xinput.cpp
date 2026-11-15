// Test-only fake XInput 1.4 provider: never packaged in the ASI archive.
// Define a minimal Windows/XInput-compatible ABI locally to prevent the SDK's
// import declaration for XInputGetState from conflicting with this export.
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include <atomic>
#include <cstring>

namespace {
struct Gamepad {
    WORD buttons;
    BYTE left_trigger;
    BYTE right_trigger;
    SHORT left_x;
    SHORT left_y;
    SHORT right_x;
    SHORT right_y;
};
struct State {
    DWORD packet_number;
    Gamepad gamepad;
};
static_assert(sizeof(State) == 16, "XInput state ABI mismatch");
constexpr WORD kX = 0x4000;
constexpr WORD kB = 0x2000;
constexpr WORD kA = 0x1000;
constexpr WORD kY = 0x8000;
constexpr WORD kLB = 0x0100;
constexpr WORD kRB = 0x0200;
}

extern "C" __declspec(dllexport)
DWORD WINAPI XInputGetState(DWORD slot, State* state) {
    if (slot != 0) return ERROR_DEVICE_NOT_CONNECTED;
    if (!state) return ERROR_BAD_ARGUMENTS;

    static std::atomic<unsigned> polls{0};
    const unsigned sample = polls.fetch_add(1, std::memory_order_relaxed);
    const unsigned phase = sample / 4;

    // Four idle polls, then one down/up cycle for X, B, A, RT,
    // Y, LB, RB, and LT (the two combat binding alternatives).
    WORD buttons = 0;
    BYTE left_trigger = 0;
    BYTE right_trigger = 0;
    switch (phase) {
    case 1: buttons = kX; break;
    case 3: buttons = kB; break;
    case 5: buttons = kA; break;
    case 7: right_trigger = 255; break;
    case 9: buttons = kY; break;
    case 11: buttons = kLB; break;
    case 13: buttons = kRB; break;
    case 15: left_trigger = 255; break;
    default: break;
    }

    std::memset(state, 0, sizeof(*state));
    state->packet_number = sample + 1;
    state->gamepad.buttons = buttons;
    state->gamepad.left_trigger = left_trigger;
    state->gamepad.right_trigger = right_trigger;
    return ERROR_SUCCESS;
}
