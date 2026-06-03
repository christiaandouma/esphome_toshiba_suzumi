cmake_minimum_required(VERSION 3.14)
project(ToshibaSuzumiTests CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)

set(COMPONENT_DIR "${CMAKE_SOURCE_DIR}/../../components/toshiba_suzumi")
set(MOCK_DIR      "${CMAKE_SOURCE_DIR}/mock_esphome")

# Suppress warnings from generated/mocked code that we don't own
add_compile_options(-Wno-unused-parameter)

# ── Checksum tests ─────────────────────────────────────────────────────────────
# Uses an inline copy of the function — no ESPHome headers needed.
add_executable(test_checksum test_checksum.cpp)
target_link_libraries(test_checksum GTest::gtest_main)

# ── Mode conversion tests ──────────────────────────────────────────────────────
# Compiles toshiba_climate_mode.cpp against the mock ESPHome headers.
add_executable(test_mode_conversions
  test_mode_conversions.cpp
  "${COMPONENT_DIR}/toshiba_climate_mode.cpp"
)
target_include_directories(test_mode_conversions PRIVATE
  "${MOCK_DIR}"       # resolves: esphome/core/log.h etc.
  "${COMPONENT_DIR}"  # resolves: toshiba_climate_mode.h, toshiba_climate.h
)
target_link_libraries(test_mode_conversions GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(test_checksum)
gtest_discover_tests(test_mode_conversions)
