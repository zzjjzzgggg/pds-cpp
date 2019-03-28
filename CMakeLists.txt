cmake_minimum_required(VERSION 3.10)
project (Example C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_BUILD_TYPE "Debug")
# set(CMAKE_CXX_COMPILER=/usr/bin/g++)
# set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-pthread")

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} $ENV{GIT_REPO}/misc/cmake)
message(STATUS "CMAKE_MODULE_PATH: " ${CMAKE_MODULE_PATH})

# required libraries from system
find_package(Threads REQUIRED)
find_package(gflags REQUIRED)
find_package(CPPLIB REQUIRED)

add_definitions(${CPPLIB_DEFINITIONS})
include_directories(${CPPLIB_INCLUDE_DIRS})

# libraries
set(LIBRARIES ${LIBRARIES} gflags ${CPPLIB_LIBRARIES})

# MESSAGE(STATUS "LIBRARIES: " ${LIBRARIES})

add_subdirectory(src)
