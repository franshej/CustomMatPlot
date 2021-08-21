cmake_minimum_required(VERSION 3.12)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(spl_plot VERSION 0.0.1)

set(SOURCE source/spl_plot.cpp
           source/spl_graph_line.cpp
           source/spl_grid.cpp
           source/spl_label.cpp)

set(INTERNAL_HEADERS include/include_internal/spl_graph_line.h
 	                 include/include_internal/spl_grid.h
                     include/include_internal/spl_label.h
                     include/include_internal/spl_utils.h)

set(PUBLIC_HEADERS include/include/spl_plot.h
                   include/include/scp_lookandfeel.h)

add_library(${PROJECT_NAME}
            STATIC
            ${SOURCE}
            ${INTERNAL_HEADERS}
            ${PUBLIC_HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC "include/include"
                                           PUBLIC "include/include_internal")

target_link_libraries(spl_plot
                      juce::juce_gui_basics)


add_subdirectory(externals)
add_subdirectory(test)