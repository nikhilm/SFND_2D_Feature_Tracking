cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)

#include(FetchContent)
#FetchContent_Declare(
#        googletest
#        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
#)
# For Windows: Prevent overriding the parent project's compiler/linker settings
#set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
#FetchContent_MakeAvailable(googletest)

set(CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")

project(camera_fusion)

find_package(OpenCV 4.1 REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIBRARY_DIRS})
add_definitions(${OpenCV_DEFINITIONS})

# Executable for create matrix exercise
add_executable (2D_feature_tracking src/matching2D_Student.cpp src/MidTermProject_Camera_Student.cpp src/ring_buffer.h)
target_link_libraries (2D_feature_tracking ${OpenCV_LIBRARIES})

#add_executable(ring_buffer_test src/ring_buffer_test.cpp src/ring_buffer.h)
#target_link_libraries(ring_buffer_test gtest gtest_main gmock)