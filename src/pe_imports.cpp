#include "acbf/pe_imports.h"

#include <algorithm>
#include <cstdint>
#include <limits>

namespace acbf::pe {
namespace {

constexpr std::size_t kMaxSections = 96;
constexpr std::size_t kMaxImports = 256;
constexpr std::size_t kMaxName = 96;
constexpr std::size_t kMinPeSize = 0x40;

class View {
public:
    View(const std::uint8_t* bytes, std::size_t size) : bytes_(bytes), size_(size) {}

    bool has(std::uint64_t offset, std::size_t count) const noexcept {
        return bytes_ != nullptr && offset <= size_ && count <= size_ - offset;
    }

    bool u16(std::uint64_t offset, std::uint16_t& value) const noexcept {
        if (!has(offset, 2)) return false;
        const auto i = static_cast<std::size_t>(offset);
        value = static_cast<std::uint16_t>(bytes_[i]) |
                static_cast<std::uint16_t>(bytes_[i + 1]) << 8;
        return true;
    }

    bool u32(std::uint64_t offset, std::uint32_t& value) const noexcept {
        if (!has(offset, 4)) return false;
        const auto i = static_cast<std::size_t>(offset);
        value = static_cast<std::uint32_t>(bytes_[i]) |
                static_cast<std::uint32_t>(bytes_[i + 1]) << 8 |
                static_cast<std::uint32_t>(bytes_[i + 2]) << 16 |
                static_cast<std::uint32_t>(bytes_[i + 3]) << 24;
        return true;
    }

    bool name(std::uint64_t offset, std::string& output) const {
        output.clear();
        for (std::size_t n = 0; n < kMaxName; ++n) {
            if (!has(offset + n, 1)) return false;
            unsigned char c = bytes_[static_cast<std::size_t>(offset + n)];
            if (c == 0) return !output.empty();
            // Only ASCII DLL basenames; reject paths and JSON punctuation.
            if ((c < 'A' || c > 'Z') && (c < 'a' || c > 'z') &&
                (c < '0' || c > '9') && c != '.' && c != '_' && c != '-')
                return false;
            if (c >= 'A' && c <= 'Z') c = static_cast<unsigned char>(c - 'A' + 'a');
            output.push_back(static_cast<char>(c));
        }
        return false;
    }

private:
    const std::uint8_t* bytes_;
    std::size_t size_;
};

struct Section {
    std::uint32_t rva;
    std::uint32_t virtual_size;
    std::uint32_t raw_size;
    std::uint32_t raw_offset;
};

bool rva_to_file(const View& view, std::uint32_t rva, std::uint32_t headers_size,
                 const std::vector<Section>& sections, std::size_t needed,
                 std::uint64_t& file_offset) noexcept {
    if (rva < headers_size && view.has(rva, needed)) {
        file_offset = rva;
        return true;
    }
    for (const Section& s : sections) {
        const std::uint64_t start = s.rva;
        const std::uint64_t span = std::max(s.virtual_size, s.raw_size);
        if (rva < start || static_cast<std::uint64_t>(rva) - start >= span) continue;
        const std::uint64_t delta = static_cast<std::uint64_t>(rva) - start;
        if (delta >= s.raw_size || needed > static_cast<std::uint64_t>(s.raw_size) - delta)
            return false;
        file_offset = static_cast<std::uint64_t>(s.raw_offset) + delta;
        return view.has(file_offset, needed);
    }
    return false;
}

}  // namespace

const char* status_name(ImportStatus value) noexcept {
    switch (value) {
    case ImportStatus::Ok: return "OK";
    case ImportStatus::NoImports: return "NO_IMPORTS";
    case ImportStatus::InvalidPe: return "INVALID_PE";
    case ImportStatus::UnsupportedPe: return "UNSUPPORTED_PE";
    case ImportStatus::MalformedImports: return "MALFORMED_IMPORTS";
    }
    return "UNKNOWN";
}

bool is_input_library(const std::string& name) noexcept {
    return name == "gameinput.dll" ||
           name == "dinput8.dll" ||
           name == "dinput.dll" ||
           name == "xinput1_4.dll" ||
           name == "xinput1_3.dll" ||
           name == "xinput9_1_0.dll" ||
           name == "xinputuap.dll" ||
           name == "xinput1_2.dll" ||
           name == "xinput1_1.dll";
}

ImportResult enumerate_imports(const std::uint8_t* bytes, std::size_t size) {
    ImportResult result;
    const View view(bytes, size);
    if (!view.has(0, kMinPeSize) || bytes[0] != 'M' || bytes[1] != 'Z')
        return result;

    std::uint32_t pe_offset = 0;
    if (!view.u32(0x3c, pe_offset) || !view.has(pe_offset, 24))
        return result;
    std::uint32_t signature = 0;
    std::uint16_t machine = 0, section_count = 0, optional_size = 0;
    if (!view.u32(pe_offset, signature) || signature != 0x4550 ||
        !view.u16(static_cast<std::uint64_t>(pe_offset) + 4, machine) ||
        !view.u16(static_cast<std::uint64_t>(pe_offset) + 6, section_count) ||
        !view.u16(static_cast<std::uint64_t>(pe_offset) + 20, optional_size))
        return result;

    const std::uint64_t optional = static_cast<std::uint64_t>(pe_offset) + 24;
    if (!section_count || section_count > kMaxSections ||
        optional_size < 128 || !view.has(optional, optional_size) ||
        !view.has(optional + optional_size, static_cast<std::size_t>(section_count) * 40))
        return result;

    std::uint16_t magic = 0;
    if (!view.u16(optional, magic)) return result;
    if (magic != 0x20b || machine != 0x8664) {
        result.status = ImportStatus::UnsupportedPe;
        return result;
    }

    std::uint32_t header_size = 0, directory_count = 0;
    std::uint32_t import_rva = 0, import_size = 0;
    if (!view.u32(optional + 60, header_size) ||
        !view.u32(optional + 108, directory_count))
        return result;

    if (directory_count < 2) {
        result.status = ImportStatus::NoImports;
        return result;
    }
    if (!view.u32(optional + 120, import_rva) ||
        !view.u32(optional + 124, import_size))
        return result;
    if (!import_rva && !import_size) {
        result.status = ImportStatus::NoImports;
        return result;
    }
    if (!import_rva || import_size < 20) {
        result.status = ImportStatus::MalformedImports;
        return result;
    }

    std::vector<Section> sections;
    sections.reserve(section_count);
    for (std::size_t i = 0; i < section_count; ++i) {
        const auto section = optional + optional_size + i * 40;
        Section s{};
        if (!view.u32(section + 8, s.virtual_size) ||
            !view.u32(section + 12, s.rva) ||
            !view.u32(section + 16, s.raw_size) ||
            !view.u32(section + 20, s.raw_offset))
            return result;
        sections.push_back(s);
    }

    const std::size_t limit = std::min<std::size_t>(import_size / 20, kMaxImports);
    bool terminated = false;
    for (std::size_t i = 0; i < limit; ++i) {
        const std::uint64_t descriptor_rva =
            static_cast<std::uint64_t>(import_rva) + i * 20;
        if (descriptor_rva > std::numeric_limits<std::uint32_t>::max()) break;
        std::uint64_t offset = 0;
        if (!rva_to_file(view, static_cast<std::uint32_t>(descriptor_rva),
                         header_size, sections, 20, offset))
            break;

        std::uint32_t words[5]{};
        bool valid = true;
        for (std::size_t w = 0; w < 5; ++w)
            valid &= view.u32(offset + w * 4, words[w]);
        if (!valid) break;
        if (!words[0] && !words[1] && !words[2] && !words[3] && !words[4]) {
            terminated = true;
            break;
        }

        std::uint64_t name_offset = 0;
        if (!rva_to_file(view, words[3], header_size, sections, 1, name_offset))
            break;
        std::string name;
        if (!view.name(name_offset, name)) break;
        result.dll_names.push_back(name);
    }

    if (!terminated) {
        result.status = ImportStatus::MalformedImports;
        result.dll_names.clear();
        return result;
    }
    std::sort(result.dll_names.begin(), result.dll_names.end());
    result.dll_names.erase(std::unique(result.dll_names.begin(), result.dll_names.end()),
                           result.dll_names.end());
    result.status = ImportStatus::Ok;
    return result;
}

}  // namespace acbf::pe
