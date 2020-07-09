cmake_minimum_required(VERSION 3.15)
project(people)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
add_executable(
        people
        main.cpp
)

target_include_directories(people PRIVATE)
target_link_libraries(people ${OpenCV_LIBS})