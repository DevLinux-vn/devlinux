cmake_minimum_required(VERSION 3.10)

project (hw_lab2 LANGUAGES C)

include(CTest)

include(FetchContent)

FetchContent_Declare(
    Unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG v2.6.1
)

FetchContent_MakeAvailable(Unity)

add_compile_options(
    -Wall
    -Wextra
    -Werror
    -pedantic
    -std=c11
)

add_library(sensor_lib STATIC
    src/sensor.c
)

target_include_directories(sensor_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

add_executable(test_sensor
    test/test_sensor.c
)

target_link_libraries(test_sensor
    PRIVATE
    sensor_lib
    unity
)

add_test(
    NAME sensor_tests
    COMMAND test_sensor
)

find_program(CLANG_TIDY clang-tidy REQUIRED)
find_program(CPPCHECK cppcheck REQUIRED)

set(SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sensor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/test/test_sensor.c
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