import pathlib
import re
import unittest


SOURCE = pathlib.Path(__file__).with_name("observer.cpp").read_text(encoding="utf-8")


class NativeHookAbiTests(unittest.TestCase):
    def test_all_native_detours_preserve_x86_thiscall_stack_contracts(self):
        contracts = [
            ("CallbackFn", r"uint32_t\(__thiscall\*\)\(void\*, uintptr_t\)",
             r"uint32_t __fastcall callback_hook\(void\* self, void\*, uintptr_t arg\)"),
            ("ResolverFn", r"void\(__thiscall\*\)\(void\*, uintptr_t, int\*, char\)",
             r"void __fastcall resolver_hook\(void\* self, void\*, uintptr_t flags, int\* output, char mode\)"),
            ("DispatchFn", r"int\(__thiscall\*\)\(void\*, uint32_t, uint32_t, int\*, uintptr_t\)",
             r"int __fastcall dispatch_hook\(void\* self, void\*, uint32_t flag_a, uint32_t flag_b,\s*int\* handler, uintptr_t opaque_arg4\)"),
            ("GateFn", r"uint32_t\(__thiscall\*\)\(void\*, uint32_t\)",
             r"uint32_t __fastcall gate_hook\(void\* self, void\*, uint32_t slot\)"),
            ("EdgeFn", r"uint32_t\(__thiscall\*\)\(void\*, uint32_t, uintptr_t, uintptr_t\)",
             r"uint32_t __fastcall edge_hook\(void\* self, void\*, uint32_t slot, uintptr_t arg3, uintptr_t arg4\)"),
            ("HoldFn", r"uint32_t\(__thiscall\*\)\(void\*, uint32_t, uintptr_t, uintptr_t, uintptr_t\)",
             r"uint32_t __fastcall hold_hook\(void\* self, void\*, uint32_t slot, uintptr_t arg3,\s*uintptr_t arg4, uintptr_t arg5\)"),
            ("AvailableFn", r"uint8_t\(__thiscall\*\)\(void\*, uint32_t\)",
             r"uint8_t __fastcall available_hook\(void\* self, void\*, uint32_t slot\)"),
        ]
        for alias, type_pattern, hook_pattern in contracts:
            with self.subTest(alias=alias):
                self.assertRegex(SOURCE, rf"using {alias} = {type_pattern};")
                self.assertRegex(SOURCE, hook_pattern)

    def test_fourth_dispatch_argument_is_forwarded_unchanged(self):
        self.assertRegex(
            SOURCE,
            r"original_dispatch\(self, flag_a, flag_b, handler, opaque_arg4\)",
        )


if __name__ == "__main__":
    unittest.main()
