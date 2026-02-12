cmake_minimum_required(VERSION 3.13)

# Include the Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(hydra_sort C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the Pico SDK
pico_sdk_init()

# =============================================================================
# HYDRA-SORT Library (Header-Only)
# =============================================================================

add_library(hydra_sort INTERFACE)
target_include_directories(hydra_sort INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# =============================================================================
# Examples
# =============================================================================

# Basic Usage Example
add_executable(basic_usage
    examples/basic_usage.c
)
target_link_libraries(basic_usage
    pico_stdlib
    pico_multicore
    hardware_dma
    hydra_sort
)
pico_add_extra_outputs(basic_usage)
pico_enable_stdio_usb(basic_usage 1)
pico_enable_stdio_uart(basic_usage 0)

# Benchmark Example
add_executable(benchmark
    examples/benchmark.c
)
target_link_libraries(benchmark
    pico_stdlib
    pico_multicore
    hardware_dma
    hydra_sort
)
pico_add_extra_outputs(benchmark)
pico_enable_stdio_usb(benchmark 1)
pico_enable_stdio_uart(benchmark 0)

# =============================================================================
# Tests
# =============================================================================

add_executable(test_correctness
    tests/test_correctness.c
)
target_link_libraries(test_correctness
    pico_stdlib
    hydra_sort
)
pico_add_extra_outputs(test_correctness)
pico_enable_stdio_usb(test_correctness 1)
pico_enable_stdio_uart(test_correctness 0)
