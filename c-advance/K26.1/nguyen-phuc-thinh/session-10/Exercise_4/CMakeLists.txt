cmake_minimum_required(VERSION 3.14)
project(exercise_4 C)

# Strict C11 compiler flags
add_compile_options(-Wall -Wextra -pedantic -Werror -std=c11)

# Main Executable
add_executable(exercise_4 src/main.c src/memory_ops.c)
target_include_directories(exercise_4 PRIVATE include)

# Unity Test Framework
include(FetchContent)
FetchContent_Declare(unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.6.0
)
FetchContent_MakeAvailable(unity)

# Test Executable
enable_testing()
add_executable(test_memory_ops test/test_memory_ops.c src/memory_ops.c)
target_include_directories(test_memory_ops PRIVATE include)
target_link_libraries(test_memory_ops PRIVATE unity)
add_test(NAME test_memory_ops COMMAND test_memory_ops)