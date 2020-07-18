add_library(juce_plot_window 
            static 
            juce_plot_window.cpp 
            juce_plot_window.h)

target_link_libraries(juce_plot_window
                      juce::juce_gui_extra)
