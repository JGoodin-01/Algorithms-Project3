cmake_minimum_required(VERSION 3.25)

# project info
project(carve_seam)

# C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Use Release build type if not specified
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_executable(carve_seam)

target_sources(carve_seam PRIVATE carve_seam.cpp ImageCarver.cpp)

configure_file(Buchtel.pgm Buchtel.pgm COPYONLY)
configure_file(bug.pgm bug.pgm COPYONLY)
configure_file(color.ppm color.ppm COPYONLY)

add_custom_target(run
        COMMENT "Run demo"
        COMMAND $<TARGET_FILE:carve_seam>
        DEPENDS carve_seam
        USES_TERMINAL
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
