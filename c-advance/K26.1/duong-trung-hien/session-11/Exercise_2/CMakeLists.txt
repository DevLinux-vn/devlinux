cmake_minimum_required(VERSION 3.14)
project (exercise_2 LANGUAGES C)

add_compile_options(
    -Wall
    -Wextra
    -Werror
    -pedantic
    -std=c11
)

add_executable(exercise_2
    src/main.c
    src/fixed_point.c
)
target_include_directories(exercise_2
    PRIVATE
    inc
)

# Unity test framework
include(FetchContent)
FetchContent_Declare(unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.6.0
)
FetchContent_MakeAvailable(unity)

# Test executable
add_executable(test_exercise_2
    test/test_fixed_point.c
    src/fixed_point.c
)
target_include_directories(test_exercise_2
    PRIVATE 
    inc
)
target_link_libraries(test_exercise_2
    PRIVATE 
    unity
)

# Runner / test executable tool
include(CTest)
add_test(
    NAME test_exercise_2
    COMMAND test_exercise_2
)

# Analystics tool
find_program(CLANG_TIDY clang-tidy REQUIRED)
find_program(CPPCHECK cppcheck REQUIRED)

set(SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fixed_point.c
    ${CMAKE_CURRENT_SOURCE_DIR}/test/test_fixed_point.c
)

add_custom_target(clang_tidy
    COMMAND ${CLANG_TIDY}
            ${SRC}
            --
            -std=c11
            -I${CMAKE_CURRENT_SOURCE_DIR}/inc
            -I${unity_SOURCE_DIR}/src
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(cpp_check
    COMMAND ${CPPCHECK}
            --enable=all
            --error-exitcode=1
            --suppress=missingIncludeSystem
            -I${CMAKE_CURRENT_SOURCE_DIR}/inc
            -I${unity_SOURCE_DIR}/src
            ${SRC}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)