// Test-only fake XInput 1.4 provider: never packaged in the ASI archive.
// This is a deterministic offline fixture, not a replacement for game input.
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <xinput.h>

#include <atomic>
#include <cstring>

extern "C" __declspec(dllexport)
DWORD WINAPI XInputGetState(DWORD slot, XINPUT_STATE* state) {
    if (slot != 0) return ERROR_DEVICE_NOT_CONNECTED;
    if (!state) return ERROR_BAD_ARGUMENTS;

    static std::atomic<unsigned> polls{0};
    const unsigned sample = polls.fetch_add(1, std::memory_order_relaxed);
    const unsigned phase = sample / 4;

    // 4 empty polls, then one complete down/up cycle for each button.
    // A and RT intentionally use distinct hardware representations.
    WORD buttons = 0;
    BYTE right_trigger = 0;
    switch (phase) {
    case 1: buttons = XINPUT_GAMEPAD_X; break;
    case 3: buttons = XINPUT_GAMEPAD_B; break;
    case 5: buttons = XINPUT_GAMEPAD_A; break;
    case 7: right_trigger = 255; break;
    default: break;
    }

    std::memset(state, 0, sizeof(*state));
    state->dwPacketNumber = sample + 1;
    state->Gamepad.wButtons = buttons;
    state->Gamepad.bRightTrigger = right_trigger;
    return ERROR_SUCCESS;
}
