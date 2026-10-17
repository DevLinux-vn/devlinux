cmake_minimum_required(VERSION 3.14)
project(exercise_2 C)

add_compile_options(-Wall -Wextra -pedantic -Werror -std=c11)

add_executable(exercise_2 src/main.c src/fixed_point.c)
target_include_directories(exercise_2 PRIVATE include)

# Unity
include(FetchContent)
FetchContent_Declare(unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.6.0
)
FetchContent_MakeAvailable(unity)

enable_testing()
add_executable(test_exercise_2 test/test_fixed_point.c src/fixed_point.c)
target_include_directories(test_exercise_2 PRIVATE include)
target_link_libraries(test_exercise_2 PRIVATE unity)
add_test(NAME test_exercise_2 COMMAND test_exercise_2)