cmake_minimum_required(VERSION 3.10)

project (hw_lab3 LANGUAGES C)

include (CTest)

include (FetchContent)

FetchContent_Declare(
    Unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG v2.6.1
)

FetchContent_MakeAvailable(Unity)

FetchContent_Declare(
    FFF
    GIT_REPOSITORY https://github.com/meekrosoft/fff.git
    GIT_TAG master
)

FetchContent_MakeAvailable(FFF)

add_compile_options(
    -Wall
    -Wextra
    -Werror
    -pedantic 
    -std=c11
)

add_library(command_handler_lib STATIC
    src/command_handler.c
)

target_include_directories(command_handler_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

add_executable(test_handler
    test/test_handler.c
)

target_link_libraries(test_handler
    PRIVATE
    command_handler_lib
    unity
    fff
)

add_test(
    NAME uart_tests
    COMMAND test_handler
)

find_program(CLANG_TIDY clang-tidy REQUIRED)
find_program(CPPCHECK cppcheck REQUIRED)

set(SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/command_handler.c
    ${CMAKE_CURRENT_SOURCE_DIR}/test/test_handler.c
)

add_custom_target(clang_tidy
    COMMAND ${CLANG_TIDY}
            ${SRC}
            --
            -std=c11
            -I${CMAKE_CURRENT_SOURCE_DIR}/inc
            -I${unity_SOURCE_DIR}/src
            -I${fff_SOURCE_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(cpp_check
    COMMAND ${CPPCHECK}
            --enable=all
            --error-exitcode=1
            --suppress=missingIncludeSystem
            -I${CMAKE_CURRENT_SOURCE_DIR}/inc
            -I${unity_SOURCE_DIR}/src
            -I${fff_SOURCE_DIR}
            ${SRC}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)