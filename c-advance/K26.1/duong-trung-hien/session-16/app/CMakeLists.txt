add_executable(app main.c)
target_link_libraries(app PRIVATE hal drivers data telemetry config app_component)
target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})