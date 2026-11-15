cmake_minimum_required(VERSION 3.20)
project(ac4_observer LANGUAGES CXX)

if(NOT WIN32 OR NOT MSVC)
  message(FATAL_ERROR "The original AC4 observer requires MSVC on Windows.")
endif()
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
  message(FATAL_ERROR "Configure this project with the Win32 generator (x86).")
endif()

include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON CACHE BOOL "Use the pinned MinHook source once fetched")
FetchContent_Declare(minhook
  GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
  GIT_TAG c3fcafdc10146beb5919319d0683e44e3c30d537
  GIT_SHALLOW FALSE)
FetchContent_MakeAvailable(minhook)

add_library(AC4ControlObserver SHARED observer.cpp)
target_compile_features(AC4ControlObserver PRIVATE cxx_std_17)
target_compile_definitions(AC4ControlObserver PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
target_include_directories(AC4ControlObserver PRIVATE ${minhook_SOURCE_DIR}/include)
target_link_libraries(AC4ControlObserver PRIVATE minhook xinput9_1_0 bcrypt)
target_compile_options(AC4ControlObserver PRIVATE /W4 /WX /permissive- /EHsc)
set_target_properties(AC4ControlObserver PROPERTIES PREFIX "" SUFFIX ".asi"
  OUTPUT_NAME "AC4ControlObserver")

set(AC4_LOCAL_PROFILE_DIR "" CACHE PATH
  "Local, untracked exact-executable profile directory. Omit to build a disarmed observer.")
if(AC4_LOCAL_PROFILE_DIR)
  if(NOT EXISTS "${AC4_LOCAL_PROFILE_DIR}/target_profile.local.h")
    message(FATAL_ERROR "The local profile header is missing from AC4_LOCAL_PROFILE_DIR.")
  endif()
  target_include_directories(AC4ControlObserver PRIVATE "${AC4_LOCAL_PROFILE_DIR}")
  target_compile_definitions(AC4ControlObserver PRIVATE
    AC4_LOCAL_PROFILE_HEADER="target_profile.local.h")
endif()

add_executable(observer_guard_tests guard_tests.cpp)
target_compile_features(observer_guard_tests PRIVATE cxx_std_17)
target_compile_definitions(observer_guard_tests PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
target_compile_options(observer_guard_tests PRIVATE /W4 /WX /permissive-)
enable_testing()
add_test(NAME observer_build_guard COMMAND observer_guard_tests)

add_executable(observer_synthetic_host_test synthetic_host_test.cpp)
target_compile_features(observer_synthetic_host_test PRIVATE cxx_std_17)
target_compile_definitions(observer_synthetic_host_test PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
target_compile_options(observer_synthetic_host_test PRIVATE /W4 /WX /permissive-)
add_dependencies(observer_synthetic_host_test AC4ControlObserver)
add_test(NAME observer_synthetic_load
  COMMAND observer_synthetic_host_test "$<TARGET_FILE:AC4ControlObserver>"
          "${CMAKE_CURRENT_BINARY_DIR}/synthetic-output")

install(TARGETS AC4ControlObserver RUNTIME DESTINATION . LIBRARY DESTINATION .)
