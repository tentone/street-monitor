cmake_minimum_required( VERSION 3.6 )

set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

find_package( Git )
execute_process(
  COMMAND ${GIT_EXECUTABLE} clone "https://github.com/ocornut/imgui.git" -b v1.72b
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
set( IMGUI_DIR ${CMAKE_CURRENT_BINARY_DIR}/imgui )

set( imgui_files
  ${IMGUI_DIR}/imconfig.h
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui.h
  ${IMGUI_DIR}/imgui_demo.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_internal.h
  ${IMGUI_DIR}/imgui_widgets.cpp
  ${IMGUI_DIR}/imstb_rectpack.h
  ${IMGUI_DIR}/imstb_textedit.h
  ${IMGUI_DIR}/imstb_truetype.h
)

set( imgui_impl_files
  ${IMGUI_DIR}/examples/imgui_impl_glfw.h
  ${IMGUI_DIR}/examples/imgui_impl_glfw.cpp
  ${IMGUI_DIR}/examples/imgui_impl_opengl3.h
  ${IMGUI_DIR}/examples/imgui_impl_opengl3.cpp
)

set( gl3w
  ${IMGUI_DIR}/examples/libs/gl3w/GL/gl3w.c
)

project( sample )
add_executable( sample ${imgui_files} ${imgui_impl_files} ${gl3w} main.cpp )

set_property( DIRECTORY PROPERTY VS_STARTUP_PROJECT "sample" )

find_package( OpenCV REQUIRED )
find_package( OpenGL REQUIRED )

include( ExternalProject )
ExternalProject_Add(
  glfw PREFIX glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.3
  CMAKE_ARGS
    "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>"
    "-DCMAKE_BUILD_TYPE=Release"
    "-DGLFW_BUILD_DOCS=OFF"
    "-DGLFW_BUILD_EXAMPLES=OFF"
    "-DGLFW_BUILD_TESTS=OFF"
)
ExternalProject_Get_Property( glfw INSTALL_DIR )
set( GLFW_DIR ${INSTALL_DIR} )
set( GLFW_INCLUDE_DIR ${GLFW_DIR}/include )
set( GLFW_LIBRARIES   ${GLFW_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}glfw3${CMAKE_STATIC_LIBRARY_SUFFIX} )
add_dependencies( sample glfw )

if( OpenCV_FOUND AND OPENGL_FOUND )
  include_directories( ${IMGUI_DIR} )
  include_directories( ${IMGUI_DIR}/examples )
  include_directories( ${IMGUI_DIR}/examples/libs/gl3w )
  include_directories( ${OPENGL_INCLUDE_DIR} )
  include_directories( ${GLFW_INCLUDE_DIR} )

  target_link_libraries( sample ${OpenCV_LIBS} )
  target_link_libraries( sample ${OPENGL_LIBRARIES} )
  target_link_libraries( sample ${GLFW_LIBRARIES} )
endif()