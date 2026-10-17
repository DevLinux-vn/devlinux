cmake_minimum_required(VERSION 3.14)

project(exercise_2 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_executable(exercise_2
    src/main.c
    src/intrusive_list.c
)

target_include_directories(exercise_2 PRIVATE include)

target_compile_options(exercise_2 PRIVATE
    -Wall
    -Wextra
    -pedantic
    -Werror
)

include(FetchContent)

FetchContent_Declare(
    unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG v2.6.0
)

FetchContent_MakeAvailable(unity)

enable_testing()

add_executable(test_exercise_2
    test/test_intrusive_list.c
    src/intrusive_list.c
)

target_include_directories(test_exercise_2 PRIVATE include)

target_compile_options(test_exercise_2 PRIVATE
    -Wall
    -Wextra
    -pedantic
    -Werror
)

target_link_libraries(test_exercise_2 PRIVATE unity)

add_test(NAME test_exercise_2 COMMAND test_exercise_2)
