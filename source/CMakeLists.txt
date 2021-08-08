add_library(spl_plot
            STATIC
            spl_plot.cpp
            spl_plot.h
            spl_graph_line.cpp
            spl_graph_line.h
            spl_grid.cpp
            spl_grid.h
            spl_label.cpp
            spl_label.h
            spl_utils.h)

target_include_directories(spl_plot PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/spl_plot >
  $<INSTALL_INTERFACE:spl_plot>  # <prefix>/spl_plot
)

target_link_libraries(spl_plot
                      juce::juce_gui_basics)
