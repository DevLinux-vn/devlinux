cmake_minimum_required(VERSION 3.14)
project(exercise_2 C)

# Strict C11 compiler flags
add_compile_options(-Wall -Wextra -pedantic -Werror -std=c11)

# Main Executable
add_executable(exercise_2 src/main.c src/command_parser.c)
target_include_directories(exercise_2 PRIVATE include)

# Unity Test Framework
include(FetchContent)
FetchContent_Declare(unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.6.0
)
FetchContent_MakeAvailable(unity)

# Test Executable
enable_testing()
add_executable(test_command_parser test/test_command_parser.c src/command_parser.c)
target_include_directories(test_command_parser PRIVATE include)
target_link_libraries(test_command_parser PRIVATE unity)
add_test(NAME test_command_parser COMMAND test_command_parser)