cmake_minimum_required(VERSION 3.16)
project(exercise_3 VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# POSIX feature test macro - required for open(), read(), write(), close()
add_compile_definitions(_POSIX_C_SOURCE=200809L)

set(SANITIZER "none" CACHE STRING "Sanitizer (none|asan|ubsan|asan+ubsan)")
set_property(CACHE SANITIZER PROPERTY STRINGS none asan ubsan "asan+ubsan")

find_program(CLANG_TIDY_EXE NAMES clang-tidy)
if(CLANG_TIDY_EXE)
    set(CMAKE_C_CLANG_TIDY ${CLANG_TIDY_EXE} --header-filter=${CMAKE_SOURCE_DIR}/include)
endif()

find_program(CPPCHECK_EXE NAMES cppcheck)
if(CPPCHECK_EXE)
    add_custom_target(cppcheck
        COMMAND ${CPPCHECK_EXE} --enable=all --inconclusive --std=c11
            --suppress=missingIncludeSystem
            --project=${CMAKE_BINARY_DIR}/compile_commands.json
            --error-exitcode=1
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif()

add_library(device_file_io STATIC src/device_file_io.c)
target_include_directories(device_file_io PUBLIC include)
target_compile_options(device_file_io PRIVATE -Wall -Wextra -pedantic -Werror)

add_executable(exercise_3 src/main.c)
target_link_libraries(exercise_3 PRIVATE device_file_io)
target_compile_options(exercise_3 PRIVATE -Wall -Wextra -pedantic -Werror)

if(SANITIZER STREQUAL "asan+ubsan")
    target_compile_options(device_file_io PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(device_file_io PUBLIC -fsanitize=address,undefined)
    target_compile_options(exercise_3 PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(exercise_3 PUBLIC -fsanitize=address,undefined)
endif()

include(FetchContent)
FetchContent_Declare(
    unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.6.0
)
FetchContent_MakeAvailable(unity)

enable_testing()
add_executable(test_exercise_3 test/test_device_file_io.c)
target_link_libraries(test_exercise_3 PRIVATE device_file_io unity)
target_compile_options(test_exercise_3 PRIVATE -Wall -Wextra -pedantic -Werror)

if(SANITIZER STREQUAL "asan+ubsan")
    target_compile_options(test_exercise_3 PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(test_exercise_3 PUBLIC -fsanitize=address,undefined)
endif()

add_test(NAME test_exercise_3 COMMAND test_exercise_3)