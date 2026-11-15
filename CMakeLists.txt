cmake_minimum_required(VERSION 3.20)
project(acbf_resynced_classic LANGUAGES CXX)

# Pure offline logic only. No ASI/DLL integration and no game dependencies.
add_library(acbf_control_core STATIC
    src/classic_control.cpp
)
target_include_directories(acbf_control_core
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_compile_features(acbf_control_core PUBLIC cxx_std_17)

if(MSVC)
    target_compile_options(acbf_control_core PRIVATE /W4 /WX /permissive-)
else()
    target_compile_options(acbf_control_core PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

enable_testing()
add_executable(acbf_control_core_tests
    tests/classic_control_tests.cpp
)
target_link_libraries(acbf_control_core_tests PRIVATE acbf_control_core)
add_test(NAME classic_control_core COMMAND acbf_control_core_tests)

# Intentionally NO install() target. This is not a playable mod and should
# not be offered as a game installation package.
