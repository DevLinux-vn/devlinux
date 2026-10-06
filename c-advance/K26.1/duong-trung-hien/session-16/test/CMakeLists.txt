include(FetchContent)
FetchContent_Declare(unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.6.0)
FetchContent_MakeAvailable(unity)

# Disable clang-tidy for third-party Unity sources
set_target_properties(unity PROPERTIES C_CLANG_TIDY "")

# Helper function: add a test linking to specific components
function(add_iot_test TEST_NAME SOURCE_FILE)
    add_executable(${TEST_NAME} ${SOURCE_FILE})
    target_link_libraries(${TEST_NAME} PRIVATE ${ARGN} unity)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endfunction()

# Test targets — each links ONLY the component(s) it tests
add_iot_test(test_hal  test_hal_sim.c  hal)
add_iot_test(test_logger  test_logger.c  logger)
add_iot_test(test_sensors_drv  test_sensor_drv.c  drivers)
add_iot_test(test_data_structures test_data_structures.c data)
add_iot_test(test_telemetry test_telemetry.c telemetry)
add_iot_test(test_app_logic test_app_logic.c app_component alert)
add_iot_test(test_config test_config.c config)