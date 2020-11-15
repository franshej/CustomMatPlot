cmake_minimum_required(VERSION 3.12)
# set_property(TARGET tgt PROPERTY CXX_STANDARD 17)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(spl_plot VERSION 0.0.1)

add_subdirectory(externals)
add_subdirectory(source)
add_subdirectory(test)
