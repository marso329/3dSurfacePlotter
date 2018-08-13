#[[
#   \file    CMakeLists.txt
#   \author  Martin Soderen (msoderen)
#   \date    30/05/2018
#   \version 0.0.1
#
#   \brief This cmake file generates a makefile that compiles the SurfacePlotter library.
#
#   \section DESCRIPTION
#
#	The library 
#	will end up in the lib folder. It is also possible to install the library in the /usr/local/lib folder
#	with sudo make install. The header will be installed in /usr/local/include
#
#]]
#To get normal installation directory which is defined in CMAKE_INSTALL_LIBDIR
include(GNUInstallDirs)

cmake_minimum_required (VERSION 3.9)

project (SurfacePlotter VERSION 0.0.1 DESCRIPTION "Library for plotting 3d surfaces in real-time")

set(CMAKE_BUILD_TYPE Release)

#I like the output files to end up in .lib
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set (CMAKE_CXX_FLAGS "-std=c++17 -g ")

set(CMAKE_SHARED_LINKER_FLAGS "-lglut -lGLU -lGL")
# The library we wants to create
add_library(SurfacePlotter SHARED
    src/SurfacePlotter.cxx src/Surface.cxx src/CommonFunction.cxx src/VectorUtils3.c
)



#Not needed but SurfacePlotter.so.1 will symlink to SurfacePlotter.so.0.0.1
set_target_properties(SurfacePlotter PROPERTIES SOVERSION 1)

set_target_properties(SurfacePlotter PROPERTIES PUBLIC_HEADER "include/SurfacePlotter.h")

target_include_directories(SurfacePlotter PUBLIC include )

#Rules on how to install it
install(TARGETS SurfacePlotter
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

#export pkg-config so others can easily import this
configure_file(SurfacePlotter.pc.in SurfacePlotter.pc @ONLY)

#install pkg-config file
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/SurfacePlotter.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

#This must be linked against boost
target_link_libraries(SurfacePlotter  LINK_PUBLIC ${CMAKE_SHARED_LINKER_FLAGS})

#To compile examples
add_subdirectory(examples)

