juce_add_gui_app(non_real_time_test_app
    PRODUCT_NAME "Non real-time test app.")

target_sources(non_real_time_test_app PRIVATE
    non_rt_test_app.cpp
    non_rt_tests.cpp)

target_compile_definitions(non_real_time_test_app PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:non_real_time_test_app,JUCE_PROJECT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:non_real_time_test_app,JUCE_VERSION>")

target_link_libraries(non_real_time_test_app PRIVATE
    juce::juce_gui_extra
    cmp_plot)
