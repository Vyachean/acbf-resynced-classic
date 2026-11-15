// Public Puppet Native PluginLoader ABI v1.
// Adapted from venvyl/puppet-native "plugin loader/PuppetNativeApi.h" (MIT).
// See THIRD_PARTY_NOTICES/PUPPET_NATIVE.md. No upstream runtime code copied.
#pragma once
#include <cstddef>
#include <cstdint>
#include <windows.h>

constexpr std::int32_t kPuppetNativeApiVersion = 1;

#pragma pack(push, 8)
struct PN_Api {
    std::int32_t version;
    void (__cdecl* Log)(const char* message);
    HMODULE (__cdecl* GetGameModule)();
    std::uintptr_t (__cdecl* GetBase)();
    std::int32_t (__cdecl* ReadInt)(const char*, const char*, std::int32_t);
    float (__cdecl* ReadFloat)(const char*, const char*, float);
    void (__cdecl* WriteInt)(const char*, const char*, std::int32_t);
};
#pragma pack(pop)

static_assert(sizeof(void*) == 8, "Puppet Native plugin requires Windows x64.");
static_assert(sizeof(PN_Api) == 56, "Puppet Native ABI v1 layout mismatch.");
static_assert(offsetof(PN_Api, Log) == 8, "Puppet Native ABI padding mismatch.");

using PN_InitFn = bool (__cdecl*)(PN_Api*);
using PN_ShutdownFn = void (__cdecl*)();
