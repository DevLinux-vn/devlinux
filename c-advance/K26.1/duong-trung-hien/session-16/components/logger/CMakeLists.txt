add_library(logger STATIC src/logger.c)
target_include_directories(logger PUBLIC include)
target_link_libraries(logger PUBLIC common)