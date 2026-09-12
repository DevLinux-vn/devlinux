cmake_minimum_required(VERSION 3.16)

project(Exercise_4 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

include(FetchContent)

# ------------------------------------------------------------
# Sensor registry library
# ------------------------------------------------------------

add_library(sensor_registry
    src/sensor_registry.c
)

target_include_directories(sensor_registry
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(sensor_registry
    PRIVATE
        -Wall
        -Wextra
        -pedantic
        -Werror
        -std=c11
)

# ------------------------------------------------------------
# Main application
# ------------------------------------------------------------

add_executable(exercise_4
    src/main.c
)

target_link_libraries(exercise_4
    PRIVATE
        sensor_registry
)

target_compile_options(exercise_4
    PRIVATE
        -Wall
        -Wextra
        -pedantic
        -Werror
        -std=c11
)

# ------------------------------------------------------------
# Unity
# ------------------------------------------------------------

FetchContent_Declare(
    unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG v2.6.1
)

FetchContent_MakeAvailable(unity)

# ------------------------------------------------------------
# Unit tests
# ------------------------------------------------------------

enable_testing()

add_executable(test_sensor_registry
    test/test_sensor_registry.c
)

target_link_libraries(test_sensor_registry
    PRIVATE
        sensor_registry
        unity
)

target_include_directories(test_sensor_registry
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(test_sensor_registry
    PRIVATE
        -Wall
        -Wextra
        -pedantic
        -Werror
        -std=c11
)

add_test(
    NAME sensor_registry_tests
    COMMAND test_sensor_registry
)