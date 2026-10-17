cmake_minimum_required(VERSION 3.14)
project(exercise_1 C)

# Strict C11 compiler flags
add_compile_options(-Wall -Wextra -pedantic -Werror -std=c11)

# Main executable
add_executable(exercise_1 src/main.c src/string_memory.c)
target_include_directories(exercise_1 PRIVATE include)

# Unity test framework
include(FetchContent)
FetchContent_Declare(unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.6.0
)
FetchContent_MakeAvailable(unity)

# Test executable
enable_testing()
add_executable(test_exercise_1 test/test_string_memory.c src/string_memory.c)
target_include_directories(test_exercise_1 PRIVATE include)
target_link_libraries(test_exercise_1 PRIVATE unity)
add_test(NAME test_exercise_1 COMMAND test_exercise_1)