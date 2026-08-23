cmake_minimum_required(VERSION 3.10)

project (hw_lab1 LANGUAGES C)

add_compile_options(
    -Wall
    -Wextra
    -pedantic
    -Werror
    -std=c11
)

add_library(crc_lib STATIC 
    src/crc_calc.c
)

target_include_directories(crc_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

add_library(ring_buffer_lib STATIC
    src/ring_buffer.c
)

target_include_directories(ring_buffer_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

add_executable(hw_lab1
    test/main.c
)

target_link_libraries(hw_lab1
    PRIVATE
    crc_lib
    ring_buffer_lib
)

find_program(CLANG_TIDY clang-tidy REQUIRED)
find_program(CPPCHECK cppcheck REQUIRED)

set(SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/crc_calc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ring_buffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/test/main.c
)

add_custom_target(clang_tidy
    COMMAND ${CLANG_TIDY}
            ${SRC}
            --
            -std=c11
            -I${CMAKE_CURRENT_SOURCE_DIR}/inc
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(cpp_check
    COMMAND ${CPPCHECK}
            --enable=all
            --error-exitcode=1
            --suppress=missingIncludeSystem
            -I${CMAKE_CURRENT_SOURCE_DIR}/inc
            ${SRC}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)