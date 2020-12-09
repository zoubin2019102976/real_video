cmake_minimum_required(VERSION 3.10)
project(GetSamples VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_subdirectory(exPackage)
list(APPEND EXTRA_LIBS ExPackage)

add_executable(GetSamples getSamples.cpp)

target_link_libraries(GetSamples PUBLIC ${EXTRA_LIBS} ${OpenCV_LIBS})
