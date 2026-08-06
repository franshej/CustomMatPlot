add_executable(cmp_projection_bench cmp_projection_bench.cpp)
# CURL::libcurl matches cmp_plot_test: on Linux juce_core references libcurl,
# so linking juce_core without it fails at link time.
target_link_libraries(cmp_projection_bench cmp_plot juce::juce_core juce::juce_events juce::juce_graphics CURL::libcurl)
target_include_directories(cmp_projection_bench PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../include/include_internal)
