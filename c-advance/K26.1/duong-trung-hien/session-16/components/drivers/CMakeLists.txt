add_library(drivers STATIC src/humi_drv.c src/temp_drv.c src/sensor_mgr.c)
target_include_directories(drivers PUBLIC include)
target_link_libraries(drivers PUBLIC common hal)