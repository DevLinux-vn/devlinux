cmake_minimum_required(VERSION 3.14)
project(exercise_3 C)

# Strict C11 compiler flags
add_compile_options(-Wall -Wextra -pedantic -Werror -std=c11)

# Main Executable
add_executable(exercise_3 src/main.c src/telemetry_codec.c)
target_include_directories(exercise_3 PRIVATE include)

# Unity Test Framework
include(FetchContent)
FetchContent_Declare(unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.6.0
)
FetchContent_MakeAvailable(unity)

# Test Executable
enable_testing()
add_executable(test_telemetry_codec test/test_telemetry_codec.c src/telemetry_codec.c)
target_include_directories(test_telemetry_codec PRIVATE include)
target_link_libraries(test_telemetry_codec PRIVATE unity)
add_test(NAME test_telemetry_codec COMMAND test_telemetry_codec)