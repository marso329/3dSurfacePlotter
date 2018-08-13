cmake_minimum_required (VERSION 3.9)
project (SurfacePlotter)

#we ned pkg-config to find the installed RBOXClient package
find_package(PkgConfig REQUIRED)
	
#Find the RBOXClient library
pkg_check_modules(RBOX REQUIRED RBOX)

# Create the Logging server executable
add_executable(realTimePlotting main.cxx)
link_directories( ../../lib )
target_include_directories(realTimePlotting PRIVATE ../../include ${RBOX_INCLUDE_DIRS} )
target_link_libraries(realTimePlotting  ${CMAKE_SHARED_LINKER_FLAGS} SurfacePlotter -lpthread ${RBOX_LIBRARIES} )
target_compile_options(realTimePlotting PUBLIC ${RBOX_CFLAGS_OTHER})