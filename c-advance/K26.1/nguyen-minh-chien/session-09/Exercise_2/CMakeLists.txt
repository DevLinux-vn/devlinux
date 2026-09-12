cmake_minimum_required(VERSION 3.14)
project(Exercise_2 C)

# ---------------------------------------------------------------------------
# Global C standard settings
# ---------------------------------------------------------------------------
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(PROJECT_STRICT_FLAGS -Wall -Wextra -pedantic -Werror -std=c11)

# ---------------------------------------------------------------------------
# Fetch the Unity test framework from GitHub
# ---------------------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
    unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.6.0
)

FetchContent_MakeAvailable(unity)

# ---------------------------------------------------------------------------
# sensor_lib: the production code under test
# ---------------------------------------------------------------------------
add_library(sensor_lib STATIC
    sensor.c
)

target_include_directories(sensor_lib
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_options(sensor_lib
    PRIVATE ${PROJECT_STRICT_FLAGS}
)

# ---------------------------------------------------------------------------
# test_sensor: Unity test executable
# ---------------------------------------------------------------------------
add_executable(test_sensor
    test_sensor.c
)

target_link_libraries(test_sensor
    PRIVATE sensor_lib
    PRIVATE unity
)

target_compile_options(test_sensor
    PRIVATE ${PROJECT_STRICT_FLAGS}
)

# ---------------------------------------------------------------------------
# Register with CTest
# ---------------------------------------------------------------------------
enable_testing()
add_test(NAME test_sensor COMMAND test_sensor)