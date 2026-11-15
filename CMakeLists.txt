cmake_minimum_required(VERSION 3.20)
project(acbf_resynced_classic LANGUAGES CXX)

# Portable, pure contextual decision engine: never invokes game actions.
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
add_executable(acbf_control_core_tests tests/classic_control_tests.cpp)
target_link_libraries(acbf_control_core_tests PRIVATE acbf_control_core)
add_test(NAME classic_control_core COMMAND acbf_control_core_tests)

if(WIN32)
    # An ASI is a DLL with a nonstandard extension, loaded by a SEPARATELY
    # installed compatible ASI loader. No loader or game binary is bundled.
    # This target observes its host process; it never patches or hooks the game.
    add_library(acbf_readonly_diagnostics SHARED src/diagnostics_win.cpp)
    target_include_directories(acbf_readonly_diagnostics
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
    target_compile_features(acbf_readonly_diagnostics PRIVATE cxx_std_17)
    target_link_libraries(acbf_readonly_diagnostics
        PRIVATE acbf_control_core bcrypt psapi)
    target_compile_definitions(acbf_readonly_diagnostics PRIVATE WIN32_LEAN_AND_MEAN)
    set_target_properties(acbf_readonly_diagnostics PROPERTIES
        PREFIX "" OUTPUT_NAME "ACBFClassicDiagnostics" SUFFIX ".asi"
    )
    install(TARGETS acbf_readonly_diagnostics RUNTIME DESTINATION .)

    # A synthetic Windows process loads the genuine .asi via LoadLibrary,
    # exercises report generation, then requests a graceful worker stop.
    add_executable(acbf_diagnostic_smoke tests/diagnostic_smoke.cpp)
    target_compile_features(acbf_diagnostic_smoke PRIVATE cxx_std_17)
    # A deterministic in-process XInput test double is injected ONLY into
    # the synthetic CI host; it is NEVER installed or distributed.
    add_library(acbf_mock_xinput SHARED tests/mock_xinput.cpp)
    set_target_properties(acbf_mock_xinput PROPERTIES
        PREFIX "" OUTPUT_NAME "xinput1_4" SUFFIX ".dll"
    )
    target_compile_features(acbf_mock_xinput PRIVATE cxx_std_17)
    add_test(NAME diagnostic_asi_smoke
        COMMAND acbf_diagnostic_smoke
            $<TARGET_FILE:acbf_readonly_diagnostics>
            $<TARGET_FILE:acbf_mock_xinput>)
    set_tests_properties(diagnostic_asi_smoke PROPERTIES TIMEOUT 40)
endif()
