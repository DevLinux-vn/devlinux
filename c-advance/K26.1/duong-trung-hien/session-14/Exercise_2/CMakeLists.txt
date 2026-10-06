cmake_minimum_required(VERSION 3.16)
project(exercise_2 VERSION 1.0.0 LANGUAGES C)

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
add_library(register_driver STATIC src/register_driver.c)
target_include_directories(register_driver PUBLIC inc)
target_compile_options(register_driver PRIVATE -Wall -Wextra -pedantic -Werror)

add_executable(exercise_2 src/main.c)
target_link_libraries(exercise_2 PRIVATE register_driver)
target_compile_options(exercise_2 PRIVATE -Wall -Wextra -pedantic -Werror)

# Apply Sanitizers
if(SANITIZER STREQUAL "asan+ubsan")
    target_compile_options(register_driver PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(register_driver PUBLIC -fsanitize=address,undefined)
    target_compile_options(exercise_2 PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(exercise_2 PUBLIC -fsanitize=address,undefined)
endif()

enable_testing()
add_executable(test_exercise_2 test/test_register_driver.c)
target_link_libraries(test_exercise_2 PRIVATE register_driver unity)
target_compile_options(test_exercise_2 PRIVATE -Wall -Wextra -pedantic -Werror)
if(SANITIZER STREQUAL "asan+ubsan")
    target_compile_options(test_exercise_2 PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(test_exercise_2 PUBLIC -fsanitize=address,undefined)
endif()
add_test(NAME test_exercise_2 COMMAND test_exercise_2)