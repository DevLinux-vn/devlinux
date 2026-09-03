cmake_minimum_required(VERSION 3.14)
project(exercise_4 C)

add_compile_options(-Wall -Wextra -pedantic -Werror -std=c11)

add_executable(exercise_4 src/main.c src/moving_average.c)
target_include_directories(exercise_4 PRIVATE include)

include(FetchContent)
FetchContent_Declare(unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.6.0
)
FetchContent_MakeAvailable(unity)

enable_testing()
add_executable(test_exercise_4 test/test_moving_average.c src/moving_average.c)
target_include_directories(test_exercise_4 PRIVATE include)
target_link_libraries(test_exercise_4 PRIVATE unity)
add_test(NAME test_exercise_4 COMMAND test_exercise_4)