include_directories(${PROJECT_SOURCE_DIR}/mysql_sd
    ${PROJECT_SOURCE_DIR}/common /usr/include/mysql/)
link_directories(${PROJECT_BINARY_DIR}/libs /usr/lib64/mysql)
set(SRC Main.cpp)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bins)
add_executable(Main ${SRC})
target_link_libraries(Main mysql_sd common)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
