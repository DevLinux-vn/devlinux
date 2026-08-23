cmake_minimum_required(VERSION 3.14)
project(exercise_4 C)

# Strict C11 compiler flags
add_compile_options(-Wall -Wextra -pedantic -Werror -std=c11)

# Main Executable
add_executable(exercise_4 src/main.c src/sensor_registry.c)
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
add_executable(test_sensor_registry test/test_sensor_registry.c src/sensor_registry.c)
target_include_directories(test_sensor_registry PRIVATE include)
target_link_libraries(test_sensor_registry PRIVATE unity)
add_test(NAME test_sensor_registry COMMAND test_sensor_registry)