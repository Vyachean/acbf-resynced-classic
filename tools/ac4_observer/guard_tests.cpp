#include <array>
#include <cstdint>
#include <cstring>
#include <iostream>

static bool matches(const uint8_t* actual, const uint8_t* expected, size_t n) {
    return actual && expected && n != 0 && std::memcmp(actual, expected, n) == 0;
}

int main() {
    constexpr std::array<uint8_t, 8> expected{0x55,0x8b,0xec,0x83,0xec,0x28,0x53,0x56};
    auto identical = expected;
    if (!matches(identical.data(), expected.data(), expected.size())) return 1;
    identical[4] ^= 1;
    if (matches(identical.data(), expected.data(), expected.size())) return 2;
    if (matches(nullptr, expected.data(), expected.size())) return 3;
    std::cout << "signature mismatch safely disables native sources\n";
    return 0;
}
