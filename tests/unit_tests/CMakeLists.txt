add_executable(cmp_plot_test cmp_main_test.cpp cmp_plot_test.cpp cmp_utils_test.cpp)
target_link_libraries(cmp_plot_test cmp_plot juce::juce_core juce::juce_events CURL::libcurl)
target_include_directories(cmp_plot_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../include/include_internal ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME cmp_plot_test COMMAND cmp_plot_test)