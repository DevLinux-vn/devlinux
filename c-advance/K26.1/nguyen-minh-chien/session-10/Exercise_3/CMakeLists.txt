cmake_minimum_required(VERSION 3.14)
project(exercise_3 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(PROJECT_STRICT_FLAGS -Wall -Wextra -pedantic -Werror -std=c11)

# ---------------------------------------------------------------------------
# Main executable
# ---------------------------------------------------------------------------
add_executable(exercise_3
    src/main.c
    src/safe_string.c
)

target_include_directories(exercise_3 PRIVATE include)

target_compile_options(exercise_3
    PRIVATE ${PROJECT_STRICT_FLAGS}
)

# ---------------------------------------------------------------------------
# Unity test framework (fetched from GitHub)
# ---------------------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
    unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.6.0
)

FetchContent_MakeAvailable(unity)

# ---------------------------------------------------------------------------
# Test executable
# ---------------------------------------------------------------------------
enable_testing()

add_executable(test_exercise_3
    test/test_safe_string.c
    src/safe_string.c
)

target_include_directories(test_exercise_3 PRIVATE include)

target_link_libraries(test_exercise_3 PRIVATE unity)

target_compile_options(test_exercise_3
    PRIVATE ${PROJECT_STRICT_FLAGS}
)

add_test(NAME test_exercise_3 COMMAND test_exercise_3)