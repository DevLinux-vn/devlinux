cmake_minimum_required(VERSION 3.16)
project(exercise_3 VERSION 1.0.0 LANGUAGES C)

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
            ${CMAKE_SOURCE_DIR}/src/telemetry.c
            ${CMAKE_SOURCE_DIR}/src/main.c
            ${CMAKE_SOURCE_DIR}/test/test_telemetry.c
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "[Quality Gate] Running cppcheck..."
    )
endif()

# Library & Main Executable
add_library(telemetry STATIC src/telemetry.c)
target_include_directories(telemetry PUBLIC inc)
target_compile_options(telemetry PRIVATE -Wall -Wextra -pedantic -Werror)

add_executable(exercise_3 src/main.c)
target_link_libraries(exercise_3 PRIVATE telemetry)
target_compile_options(exercise_3 PRIVATE 
    -Wall -Wextra -pedantic -Werror -std=c11
    -g3
    -O0
    -fno-omit-frame-pointer
)

enable_testing()
add_executable(test_exercise_3 test/test_telemetry.c)
target_link_libraries(test_exercise_3 PRIVATE telemetry unity)
target_compile_options(test_exercise_3 PRIVATE -Wall -Wextra -pedantic -Werror)

# Apply Sanitizers
if(SANITIZER STREQUAL "asan+ubsan")
    target_compile_options(telemetry PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(telemetry PUBLIC -fsanitize=address,undefined)
    target_compile_options(exercise_3 PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(exercise_3 PUBLIC -fsanitize=address,undefined)
endif()

if(SANITIZER STREQUAL "asan+ubsan")
    target_compile_options(test_exercise_3 PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(test_exercise_3 PUBLIC -fsanitize=address,undefined)
endif()

add_test(NAME test_exercise_3 COMMAND test_exercise_3)