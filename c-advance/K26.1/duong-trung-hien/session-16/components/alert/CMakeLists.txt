add_library(alert STATIC src/alert.c)
target_include_directories(alert PUBLIC include)
target_link_libraries(alert PUBLIC common logger)