#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace acbf::pe {

// A static PE IMPORT TABLE observation is not evidence that an API is used.
// No game addresses, hook signatures or third-party code are included.
enum class ImportStatus {
    Ok,
    NoImports,
    InvalidPe,
    UnsupportedPe,
    MalformedImports
};

struct ImportResult {
    ImportStatus status = ImportStatus::InvalidPe;
    std::vector<std::string> dll_names;
};

ImportResult enumerate_imports(const std::uint8_t* bytes, std::size_t size);
const char* status_name(ImportStatus value) noexcept;
bool is_input_library(const std::string& ascii_lower_name) noexcept;

}  // namespace acbf::pe
