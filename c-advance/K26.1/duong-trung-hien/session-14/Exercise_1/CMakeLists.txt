cmake_minimum_required(VERSION 3.16)
project(exercise_1 VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# POSIX feature test macro — required for open(), read(), write(), fcntl()
add_compile_definitions(_POSIX_C_SOURCE=200809L)

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
set_target_properties(unity PROPERTIES C_CLANG_TIDY "")

# Gate 2: cppcheck custom target
find_program(CPPCHECK_EXE NAMES cppcheck)
if(CPPCHECK_EXE)
    add_custom_target(cppcheck
        COMMAND ${CPPCHECK_EXE}
            --enable=all --inconclusive --std=c11
            --suppress=missingIncludeSystem
            --project=${CMAKE_BINARY_DIR}/compile_commands.json
            --error-exitcode=1
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "[Quality Gate] Running cppcheck..."
    )
endif()

# Library & Main Executable
add_library(posix_logger STATIC src/posix_logger.c)
target_include_directories(posix_logger PUBLIC inc)
target_compile_options(posix_logger PRIVATE -Wall -Wextra -pedantic -Werror)

add_executable(exercise_1 src/main.c)
target_link_libraries(exercise_1 PRIVATE posix_logger)
target_compile_options(exercise_1 PRIVATE -Wall -Wextra -pedantic -Werror)

# Apply Sanitizers
if(SANITIZER STREQUAL "asan+ubsan")
    target_compile_options(posix_logger PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(posix_logger PUBLIC -fsanitize=address,undefined)
    target_compile_options(exercise_1 PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(exercise_1 PUBLIC -fsanitize=address,undefined)
endif()

enable_testing()
add_executable(test_exercise_1 test/test_posix_logger.c)
target_link_libraries(test_exercise_1 PRIVATE posix_logger unity)
target_compile_options(test_exercise_1 PRIVATE -Wall -Wextra -pedantic -Werror)
if(SANITIZER STREQUAL "asan+ubsan")
    target_compile_options(test_exercise_1 PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(test_exercise_1 PUBLIC -fsanitize=address,undefined)
endif()
add_test(NAME test_exercise_1 COMMAND test_exercise_1)