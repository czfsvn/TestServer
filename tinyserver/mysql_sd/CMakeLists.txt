include_directories(/usr/include/mysql/ ${PROJECT_SOURCE_DIR}/common)
link_directories(/usr/lib64/mysql ${PROJECT_BINARY_DIR}/libs)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/libs)
set(LIB_SRC DBModule.cpp MySqlClient.cpp MySqlConnection.cpp MySqlRecordSet.cpp)
add_library(mysql_sd ${LIB_SRC})
target_link_libraries(mysql_sd mysqlclient_r)


SET(CMAKE_BUILD_TYPE "Debug")  
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")  
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall") 
