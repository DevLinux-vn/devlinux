cmake_minimum_required(VERSION 3.14)
project(exercise_1 C)

# Strict C11 compiler flags
add_compile_options(-Wall -Wextra -pedantic -Werror -std=c11)

# Main Executable
add_executable(exercise_1 src/main.c src/float_inspector.c)
target_include_directories(exercise_1 PRIVATE include)
target_link_libraries(exercise_1 PRIVATE m)

# Unity Test Framework
include(FetchContent)
FetchContent_Declare(unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.6.0
)
FetchContent_MakeAvailable(unity)

# Test Executable
enable_testing()
add_executable(test_float_inspector test/test_float_inspector.c src/float_inspector.c)
target_include_directories(test_float_inspector PRIVATE include)
target_link_libraries(test_float_inspector PRIVATE unity m)
add_test(NAME test_float_inspector COMMAND test_float_inspector)