add_library(app_component STATIC src/app_fsm.c src/cli_cmd.c)
target_include_directories(app_component PUBLIC include)
target_link_libraries(app_component PUBLIC common logger)