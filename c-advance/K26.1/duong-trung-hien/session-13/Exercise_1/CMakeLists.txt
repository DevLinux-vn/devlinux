cmake_minimum_required(VERSION 3.16)
project(exercise_1 VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Option: Sanitizers
set(SANITIZER "none" CACHE STRING "Sanitizer (none|asan|ubsan|asan+ubsan)")
set_property(CACHE SANITIZER PROPERTY STRINGS none asan ubsan "asan+ubsan")

# Gate 1: clang-tidy integration
find_program(CLANG_TIDY_EXE NAMES clang-tidy)
if(CLANG_TIDY_EXE)
    set(CMAKE_C_CLANG_TIDY ${CLANG_TIDY_EXE} --header-filter=${CMAKE_SOURCE_DIR}/inc)
    message(STATUS "[Quality Gate] clang-tidy: ENABLED")
endif()

# Gate 3: Unity Unit Tests via FetchContent
include(FetchContent)
FetchContent_Declare(
    unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.6.0
)
FetchContent_MakeAvailable(unity)
if(TARGET unity)
    set_target_properties(unity PROPERTIES
        C_CLANG_TIDY ""
    )
endif()

# Gate 2: cppcheck custom target
find_program(CPPCHECK_EXE NAMES cppcheck)
if(CPPCHECK_EXE)
    add_custom_target(cppcheck
        COMMAND ${CPPCHECK_EXE}
            --enable=all
            --inconclusive
            --std=c11
            --suppress=missingIncludeSystem
            --error-exitcode=1
            -I ${CMAKE_SOURCE_DIR}/inc
            -I ${unity_SOURCE_DIR}/src
            ${CMAKE_SOURCE_DIR}/src/ring_buffer.c
            ${CMAKE_SOURCE_DIR}/src/main.c
            ${CMAKE_SOURCE_DIR}/test/test_ring_buffer.c
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "[Quality Gate] Running cppcheck..."
    )
endif()

# Library & Main Executable
add_library(ring_buffer STATIC src/ring_buffer.c)
target_include_directories(ring_buffer PUBLIC inc)
target_compile_options(ring_buffer PRIVATE -Wall -Wextra -pedantic -Werror)

add_executable(exercise_1 src/main.c)
target_link_libraries(exercise_1 PRIVATE ring_buffer)
target_compile_options(exercise_1 PRIVATE -Wall -Wextra -pedantic -Werror)

enable_testing()
add_executable(test_exercise_1 test/test_ring_buffer.c)
target_link_libraries(test_exercise_1 PRIVATE ring_buffer unity)
target_compile_options(test_exercise_1 PRIVATE -Wall -Wextra -pedantic -Werror)

# Apply Sanitizers
if(SANITIZER STREQUAL "asan+ubsan")
    target_compile_options(ring_buffer PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(ring_buffer PUBLIC -fsanitize=address,undefined)
    target_compile_options(exercise_1 PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(exercise_1 PUBLIC -fsanitize=address,undefined)
endif()

if(SANITIZER STREQUAL "asan+ubsan")
    target_compile_options(test_exercise_1 PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(test_exercise_1 PUBLIC -fsanitize=address,undefined)
endif()

add_test(NAME test_exercise_1 COMMAND test_exercise_1)