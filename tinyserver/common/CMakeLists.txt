set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/libs)
set(LIB_SRC Sync.cpp System.cpp Thread.cpp)
add_library(common ${LIB_SRC})

SET(CMAKE_BUILD_TYPE "Debug")  
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")  
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall") 
