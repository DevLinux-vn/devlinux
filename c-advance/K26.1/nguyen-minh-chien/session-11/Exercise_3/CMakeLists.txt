cmake_minimum_required(VERSION 3.14)
project(exercise_3 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---------------------------------------------------------------------------
# Main application
# ---------------------------------------------------------------------------
add_executable(exercise_3
    src/main.c
    src/telemetry_codec.c
)

target_include_directories(exercise_3 PRIVATE include)

target_compile_options(exercise_3 PRIVATE
    -Wall -Wextra -pedantic -Werror -std=c11
)

# ---------------------------------------------------------------------------
# Unity test framework (fetched from GitHub)
# ---------------------------------------------------------------------------
include(FetchContent)
FetchContent_Declare(unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.6.0
)
FetchContent_MakeAvailable(unity)

enable_testing()

add_executable(test_exercise_3
    test/test_telemetry_codec.c
    src/telemetry_codec.c
)

target_include_directories(test_exercise_3 PRIVATE include)

target_compile_options(test_exercise_3 PRIVATE
    -Wall -Wextra -pedantic -Werror -std=c11
)

target_link_libraries(test_exercise_3 PRIVATE unity)

add_test(NAME test_exercise_3 COMMAND test_exercise_3)