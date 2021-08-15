cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-O3")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

project(Block_On VERSION 0.1.0)

list(APPEND CMAKE_PREFIX_PATH "cpp/include/extern/pybind11/")

include_directories("${CMAKE_SOURCE_DIR}/cpp/include/intern")
include_directories("${CMAKE_SOURCE_DIR}/cpp/include/extern")
include_directories("${CMAKE_SOURCE_DIR}/cpp/include/extern/pybind11")
include_directories("${CMAKE_SOURCE_DIR}/python")

file (GLOB SOURCE_FILES "cpp/src/*.cpp")
file (GLOB HEADER_FILES "cpp/include/intern/*.hpp" "cpp/include/extern/pybind11/*.hpp" "cpp/include/extern/glm-master/*.hpp")
file (GLOB PYTHON_FILES "python/*.cpp" "python/*.hpp")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES} ${HEADER_FILES} ${PYTHON_FILES} )

find_package(pybind11 CONFIG)
add_subdirectory(cpp/include/extern/pybind11)

#PYBIND11_ADD_MODULE(Block_On
#	${SOURCE_FILES}
#	${HEADER_FILES}
#	${PYTHON_FILES}
#)

PYBIND11_ADD_MODULE(Block_On python/block_on.cpp)

target_link_libraries(Block_On PUBLIC)

install(
  TARGETS Block_On
  COMPONENT python
  LIBRARY DESTINATION "python/lib"
)