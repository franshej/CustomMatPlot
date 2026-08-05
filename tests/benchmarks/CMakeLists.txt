add_executable(cmp_projection_bench cmp_projection_bench.cpp)
target_link_libraries(cmp_projection_bench cmp_plot juce::juce_core juce::juce_events juce::juce_graphics)
target_include_directories(cmp_projection_bench PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../include/include_internal)
