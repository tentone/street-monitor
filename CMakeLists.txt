cmake_minimum_required(VERSION 2.8)

project( Stenographer )

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( Stenographer source/main.cpp )
target_link_libraries( Stenographer ${OpenCV_LIBS} )