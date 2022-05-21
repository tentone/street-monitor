cmake_minimum_required(VERSION 3.2)

project( speed-camera )

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( speed-camera source/main.cpp )
target_link_libraries( speed-camera ${OpenCV_LIBS} )