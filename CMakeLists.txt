cmake_minimum_required(VERSION 3.2)
set(CMAKE_BUILD_TYPE Debug)
set (CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -ggdb -Wall -Wextra")
project(DAF)

find_package(OpenCV REQUIRED)

include_directories(include)
include_directories(${OpenCV_INCLUDE_DIRS} )

file(GLOB Src_Sources "src/*.cpp")
file(GLOB Test_Sources "test/*.cpp")

add_executable(morpho_test  ${Src_Sources} ${CMAKE_CURRENT_SOURCE_DIR}/test/morpho_test.cpp )
add_executable(warp_test  ${Src_Sources} ${CMAKE_CURRENT_SOURCE_DIR}/test/warp_test.cpp )
add_executable(showCaseImageEffects  ${Src_Sources} ${CMAKE_CURRENT_SOURCE_DIR}/test/showCaseImageEffects.cpp )
add_executable(optimization  ${Src_Sources} ${CMAKE_CURRENT_SOURCE_DIR}/test/optimization.cpp )

target_link_libraries( morpho_test ${OpenCV_LIBS} )
target_link_libraries( warp_test ${OpenCV_LIBS} )
target_link_libraries( showCaseImageEffects ${OpenCV_LIBS} )
target_link_libraries( optimization ${OpenCV_LIBS} )
