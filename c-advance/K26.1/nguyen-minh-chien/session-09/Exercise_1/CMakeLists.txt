cmake_minimum_required(VERSION 3.10)
project(Exercise_1 C)

# ---------------------------------------------------------------------------
# Global C standard settings
# ---------------------------------------------------------------------------
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# Strict compiler flags applied to every target in this project
# ---------------------------------------------------------------------------
set(PROJECT_STRICT_FLAGS -Wall -Wextra -pedantic -Werror -std=c11)

# ---------------------------------------------------------------------------
# crc_lib: static library for CRC-8 calculation
# ---------------------------------------------------------------------------
add_library(crc_lib STATIC
    src/crc_calc.c
)

target_include_directories(crc_lib
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(crc_lib
    PRIVATE ${PROJECT_STRICT_FLAGS}
)

# ---------------------------------------------------------------------------
# ring_buf_lib: static library for the ring buffer
# ---------------------------------------------------------------------------
add_library(ring_buf_lib STATIC
    src/ring_buffer.c
)

target_include_directories(ring_buf_lib
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(ring_buf_lib
    PRIVATE ${PROJECT_STRICT_FLAGS}
)

# ---------------------------------------------------------------------------
# hw_lab1: main executable, links both static libraries
# ---------------------------------------------------------------------------
add_executable(hw_lab1
    src/main.c
)

target_link_libraries(hw_lab1
    PRIVATE crc_lib
    PRIVATE ring_buf_lib
)

target_compile_options(hw_lab1
    PRIVATE ${PROJECT_STRICT_FLAGS}
)