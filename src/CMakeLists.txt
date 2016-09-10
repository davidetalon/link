# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.4)

# Project's name, version, and languages
project(Link VERSION 1.0 LANGUAGES C)




# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)



# Create a library called "Master" which includes the source file "master.c".
add_library (Master SHARED master.c)

# Create a library called "Sleave" which includes the source file "sleave.c".
add_library (Sleave SHARED sleave.c)




# Add executable called "link" that is built from the source file "link.c"
add_executable (link link.c)

# Link the executable to the Master library.
target_link_libraries (link LINK_PUBLIC Master)

# Link the executable to the Sleave library.
target_link_libraries (link LINK_PUBLIC Sleave)