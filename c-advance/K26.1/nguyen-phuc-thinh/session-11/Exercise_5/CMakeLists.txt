cmake_minimum_required(VERSION 3.14)
project(exercise_5 C)

# Strict C11 compiler flags
add_compile_options(-Wall -Wextra -pedantic -Werror -std=c11)

# Main Executable
add_executable(exercise_5 src/main.c src/wide_string_utils.c)
target_include_directories(exercise_5 PRIVATE include)

# Unity Test Framework
include(FetchContent)
FetchContent_Declare(unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.6.0
)
FetchContent_MakeAvailable(unity)

# Test Executable
enable_testing()
add_executable(test_wide_string_utils test/test_wide_string_utils.c src/wide_string_utils.c)
target_include_directories(test_wide_string_utils PRIVATE include)
target_link_libraries(test_wide_string_utils PRIVATE unity)
add_test(NAME test_wide_string_utils COMMAND test_wide_string_utils)