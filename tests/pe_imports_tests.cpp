#include "acbf/pe_imports.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

using acbf::pe::ImportStatus;
using acbf::pe::enumerate_imports;

namespace {
int failures = 0;
void expect(bool success, const char* label) {
    if (!success) { ++failures; std::cerr << "FAIL: " << label << '\n'; }
}
void put16(std::vector<std::uint8_t>& data, std::size_t at, std::uint16_t v) {
    data.at(at) = static_cast<std::uint8_t>(v);
    data.at(at + 1) = static_cast<std::uint8_t>(v >> 8);
}
void put32(std::vector<std::uint8_t>& data, std::size_t at, std::uint32_t v) {
    for (int i = 0; i < 4; ++i)
        data.at(at + i) = static_cast<std::uint8_t>(v >> (8 * i));
}
std::vector<std::uint8_t> fixture() {
    std::vector<std::uint8_t> pe(0x500, 0);
    pe[0] = 'M'; pe[1] = 'Z';
    put32(pe, 0x3c, 0x80);
    put32(pe, 0x80, 0x4550);
    put16(pe, 0x84, 0x8664); // x64
    put16(pe, 0x86, 1); // sections
    put16(pe, 0x94, 0xf0); // optional size
    const std::size_t opt = 0x98;
    put16(pe, opt, 0x20b); // PE32+
    put32(pe, opt + 60, 0x200); // header size
    put32(pe, opt + 108, 16); // data directory count
    put32(pe, opt + 120, 0x1000); // import RVA
    put32(pe, opt + 124, 40); // two descriptors: one import and terminal
    const std::size_t s = opt + 0xf0;
    put32(pe, s + 8, 0x200); // virtual size
    put32(pe, s + 12, 0x1000); // section RVA
    put32(pe, s + 16, 0x200); // raw size
    put32(pe, s + 20, 0x200); // raw offset
    put32(pe, 0x200 + 12, 0x1050); // name RVA in import descriptor
    const std::string name = "XInput1_4.dll";
    for (std::size_t i = 0; i < name.size(); ++i)
        pe.at(0x250 + i) = static_cast<std::uint8_t>(name[i]);
    return pe;
}
}

int main() {
    auto data = fixture();
    auto a = enumerate_imports(data.data(), data.size());
    expect(a.status == ImportStatus::Ok, "valid fixture");
    expect(a.dll_names.size() == 1 && a.dll_names[0] == "xinput1_4.dll",
           "ASCII import name normalized");
    expect(acbf::pe::is_input_library("xinput1_4.dll"), "known XInput library");
    expect(acbf::pe::is_input_library("gameinput.dll"), "known GameInput library");
    expect(!acbf::pe::is_input_library("kernel32.dll"), "unrelated module");

    data[0] = 0;
    expect(enumerate_imports(data.data(), data.size()).status == ImportStatus::InvalidPe,
           "invalid DOS header rejected");
    data = fixture();
    put16(data, 0x98, 0x10b);
    expect(enumerate_imports(data.data(), data.size()).status == ImportStatus::UnsupportedPe,
           "non-PE32+ excluded");
    data = fixture();
    put32(data, 0x98 + 120, 0);
    put32(data, 0x98 + 124, 0);
    expect(enumerate_imports(data.data(), data.size()).status == ImportStatus::NoImports,
           "absent directory not evidence of API absence");
    data = fixture();
    put32(data, 0x200 + 12, 0xfffffff0u);
    expect(enumerate_imports(data.data(), data.size()).status == ImportStatus::MalformedImports,
           "invalid import name RVA rejected");
    data = fixture();
    put32(data, 0x98 + 124, 20); // no terminal
    expect(enumerate_imports(data.data(), data.size()).status == ImportStatus::MalformedImports,
           "missing terminal descriptor rejected");
    data = fixture();
    put32(data, 0x188 + 20, 0x10000); // raw data beyond file
    expect(enumerate_imports(data.data(), data.size()).status == ImportStatus::MalformedImports,
           "out-of-file section rejected");
    data = fixture();
    for (std::size_t i = 0; i < 96; ++i) data[0x250 + i] = 'A';
    expect(enumerate_imports(data.data(), data.size()).status == ImportStatus::MalformedImports,
           "nonterminated import name rejected");
    data = fixture();
    expect(enumerate_imports(data.data(), 4).status == ImportStatus::InvalidPe,
           "truncated executable rejected");
    expect(enumerate_imports(nullptr, 0).status == ImportStatus::InvalidPe,
           "null input rejected");
    std::cout << "PE static-import test failures: " << failures << '\n';
    return failures == 0 ? 0 : 1;
}
