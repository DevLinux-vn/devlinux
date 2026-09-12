cmake_minimum_required(VERSION 3.14)
project(Exercise_3 C)

# ---------------------------------------------------------------------------
# Global C standard settings
# ---------------------------------------------------------------------------
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(PROJECT_STRICT_FLAGS -Wall -Wextra -pedantic -Werror -std=c11)

# ---------------------------------------------------------------------------
# Fetch Unity (test framework) and FFF (Fake Function Framework)
# ---------------------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
    unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.6.0
)

FetchContent_Declare(
    fff
    GIT_REPOSITORY https://github.com/meekrosoft/fff.git
    GIT_TAG        master
)

FetchContent_MakeAvailable(unity fff)

# ---------------------------------------------------------------------------
# test_handler: exercises command_handler.c against FAKE uart_hal
# functions (no real uart_hal.c is compiled or linked - this is the
# "link seam").
# ---------------------------------------------------------------------------
add_executable(test_handler
    test_handler.c
    command_handler.c
)

target_include_directories(test_handler
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(test_handler
    PRIVATE unity
    PRIVATE fff
)

target_compile_options(test_handler
    PRIVATE ${PROJECT_STRICT_FLAGS}
)

# ---------------------------------------------------------------------------
# Register with CTest
# ---------------------------------------------------------------------------
enable_testing()
add_test(NAME test_handler COMMAND test_handler)