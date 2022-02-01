juce_add_gui_app(NonRTTestApp
    PRODUCT_NAME "Non real-time test app.")

target_sources(NonRTTestApp PRIVATE
    non_rt_test_app.cpp
    non_rt_tests.cpp)

target_compile_definitions(NonRTTestApp PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:NonRTTestApp,JUCE_PROJECT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:NonRTTestApp,JUCE_VERSION>")

target_link_libraries(NonRTTestApp PRIVATE
    juce::juce_gui_extra
    cmp_plot
    test_utils)
