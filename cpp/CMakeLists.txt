cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD_17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(Block_On VERSION 0.1.0)

include(CTest)
enable_testing()

add_library(Block_On src/block_on.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

#include dir
include_directories(cpp/include)

#src
AUX_SOURCE_DIRECTORY(src SRC_FILES)

#headers
set(PROJECT_SOURCE_DIR "src")
set(PROJECT_INCLUDE_DIR "include")

#source files
set(SOURCE_FILES 
    ${PROJECT_INCLUDE_DIR}/voxelization.hpp
    ${PROJECT_SOURCE_DIR}/block_on.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})

#add library
add_library(Block_On SHARED ${SOURCE_FILES})

#include directories
target_include_directories(Block_On PRIVATE include/intern)
target_include_directories(Block_On PRIVATE include/extern)

#install
install(TARGETS Block_On DESTINATION lib)

#install the headers
install(FILES include/intern/{file} DESTINATION include)
install(FILES include/extern/{file} DESTINATION include)

#create base directory
install(DIRECTORY include/intern DESTINATION include)
install(DIRECTORY include/extern DESTINATION include)