add_library(spl_plot
            STATIC
            spl_plot.cpp
            spl_plot.h)

target_include_directories(spl_plot PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/spl_plot >
  $<INSTALL_INTERFACE:spl_plot>  # <prefix>/spl_plot
)

target_link_libraries(spl_plot
                      juce::juce_gui_basics)
