cmake_minimum_required(VERSION 3.14)
project(exercise_1 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# Main executable
# ---------------------------------------------------------------------------
add_executable(exercise_1
    src/main.c
    src/float_inspector.c
)

target_include_directories(exercise_1 PRIVATE include)

# Strict compiler flags applied via target_compile_options(), as required.
target_compile_options(exercise_1 PRIVATE
    -Wall
    -Wextra
    -pedantic
    -Werror
    -std=c11
)

target_link_libraries(exercise_1 PRIVATE m)

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

add_executable(test_exercise_1
    test/test_float_inspector.c
    src/float_inspector.c
)

target_include_directories(test_exercise_1 PRIVATE include)

target_compile_options(test_exercise_1 PRIVATE
    -Wall
    -Wextra
    -pedantic
    -Werror
    -std=c11
)

target_link_libraries(test_exercise_1 PRIVATE unity m)

add_test(NAME test_exercise_1 COMMAND test_exercise_1)