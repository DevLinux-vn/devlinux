cmake_minimum_required(VERSION 3.16)

project(Exercise_5 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

include(FetchContent)

# ------------------------------------------------------------
# Wide string utility library
# ------------------------------------------------------------

add_library(wide_string_utils
    src/wide_string_utils.c
)

target_include_directories(wide_string_utils
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(wide_string_utils
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

add_executable(exercise_5
    src/main.c
)

target_link_libraries(exercise_5
    PRIVATE
        wide_string_utils
)

target_compile_options(exercise_5
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

add_executable(test_wide_string_utils
    test/test_wide_string_utils.c
)

target_link_libraries(test_wide_string_utils
    PRIVATE
        wide_string_utils
        unity
)

target_include_directories(test_wide_string_utils
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(test_wide_string_utils
    PRIVATE
        -Wall
        -Wextra
        -pedantic
        -Werror
        -std=c11
)

add_test(
    NAME wide_string_utils_tests
    COMMAND test_wide_string_utils
)