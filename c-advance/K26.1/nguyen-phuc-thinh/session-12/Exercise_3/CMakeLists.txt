cmake_minimum_required(VERSION 3.14)
project(exercise_3 C)

add_compile_options(-Wall -Wextra -pedantic -Werror -std=c11)

add_executable(exercise_3 src/main.c src/ring_buffer_lf.c)
target_include_directories(exercise_3 PRIVATE include)

include(FetchContent)
FetchContent_Declare(unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.6.0
)
FetchContent_MakeAvailable(unity)

enable_testing()
add_executable(test_exercise_3 test/test_ring_buffer_lf.c src/ring_buffer_lf.c)
target_include_directories(test_exercise_3 PRIVATE include)
target_link_libraries(test_exercise_3 PRIVATE unity)
add_test(NAME test_exercise_3 COMMAND test_exercise_3)