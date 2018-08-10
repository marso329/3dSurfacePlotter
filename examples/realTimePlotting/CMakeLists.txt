cmake_minimum_required (VERSION 3.9)
project (SurfacePlotter)

# Create the Logging server executable
add_executable(realTimePlotting main.cxx)
link_directories( ../../lib )
target_include_directories(realTimePlotting PRIVATE ../../include )
target_link_libraries(realTimePlotting  ${CMAKE_SHARED_LINKER_FLAGS} SurfacePlotter -lpthread )
