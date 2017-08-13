cmake_minimum_required(VERSION 2.8)

project(Main)
add_subdirectory(test)
add_subdirectory(common)
add_subdirectory(mysql_sd)

#message("PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR}")

# COMMAND COMMENT 需要大写
add_custom_target(ctags
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} 
    COMMAND ctags -R -h ".c.cpp.h" -o tags
    COMMENT '生成tags'
    )

