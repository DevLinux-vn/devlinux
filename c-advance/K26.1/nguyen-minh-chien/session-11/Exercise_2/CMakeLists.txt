cmake_minimum_required(VERSION 3.14)
project(exercise_2 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# Main executable
# ---------------------------------------------------------------------------
add_executable(exercise_2
    src/main.c
    src/fixed_point.c
)

target_include_directories(exercise_2 PRIVATE include)

# Strict compiler flags applied via target_compile_options(), as required.
target_compile_options(exercise_2 PRIVATE
    -Wall
    -Wextra
    -pedantic
    -Werror
    -std=c11
)

# ---------------------------------------------------------------------------
# Unity test framework
# ---------------------------------------------------------------------------
include(FetchContent)
FetchContent_Declare(unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.6.0
)
FetchContent_MakeAvailable(unity)

# ---------------------------------------------------------------------------
# Test executable
# ---------------------------------------------------------------------------
enable_testing()

add_executable(test_exercise_2
    test/test_fixed_point.c
    src/fixed_point.c
)

target_include_directories(test_exercise_2 PRIVATE include)

target_compile_options(test_exercise_2 PRIVATE
    -Wall
    -Wextra
    -pedantic
    -Werror
    -std=c11
)

target_link_libraries(test_exercise_2 PRIVATE unity m)

add_test(NAME test_exercise_2 COMMAND test_exercise_2)