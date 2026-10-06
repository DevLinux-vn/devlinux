add_library(data STATIC src/data_proc.c src/event_log.c src/ring_buffer.c)
target_include_directories(data PUBLIC include)
target_link_libraries(data PUBLIC common)