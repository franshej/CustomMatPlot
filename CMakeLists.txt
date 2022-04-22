cmake_minimum_required(VERSION 3.12)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Get the latest version
execute_process(
    COMMAND git describe --tags --abbrev=0
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE GIT_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )

project(cmp_plot VERSION ${GIT_VERSION})

# Generate version header
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/include/cmp_version.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/include/include/cmp_version.h @ONLY)

set(SOURCE source/cmp_plot.cpp
           source/cmp_graph_line.cpp
           source/cmp_grid.cpp
           source/cmp_label.cpp
           source/cmp_frame.cpp
           source/cmp_legend.cpp
           source/cmp_trace.cpp
           source/cmp_zoom.cpp
           source/cmp_lookandfeel.cpp
           source/cmp_downsampler.cpp)

set(INTERNAL_HEADERS include/include_internal/cmp_graph_line.h
 	                 include/include_internal/cmp_grid.h
                     include/include_internal/cmp_label.h
                     include/include_internal/cmp_utils.h
                     include/include_internal/cmp_frame.h
                     include/include_internal/cmp_legend.h
                     include/include_internal/cmp_trace.h
                     include/include_internal/cmp_zoom.h
                     include/include_internal/cmp_downsampler.h)

set(PUBLIC_HEADERS include/include/cmp_plot.h
                   include/include/cmp_lookandfeel.h
                   include/include/cmp_datamodels.h)

add_library(${PROJECT_NAME}
            STATIC
            ${SOURCE})

target_include_directories(${PROJECT_NAME} PUBLIC "include/include"
                                           PRIVATE "include/include_internal")

target_include_directories(${PROJECT_NAME} PUBLIC 
                           ${CMAKE_CURRENT_BINARY_DIR}/include/include)

target_link_libraries(${PROJECT_NAME} 
                      juce::juce_gui_basics)


add_subdirectory(externals)

set(CMP_BUILD_TESTS OFF CACHE BOOL "Build tests?")
set(CMP_BUILD_EXAMPLES OFF CACHE BOOL "Build examples? requires a compiler supporting c++20.")

if(CMP_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(CMP_BUILD_TESTS)
    add_subdirectory(test)
endif()
