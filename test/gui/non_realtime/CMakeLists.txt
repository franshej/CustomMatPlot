juce_add_gui_app(NonRTTestApp
    PRODUCT_NAME "Non real-time test app.")

target_sources(NonRTTestApp PRIVATE
    non_rt_test_app.cpp
    non_rt_tests.cpp)

target_compile_definitions(NonRTTestApp PRIVATE
    # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
    JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_gui_app` call
    JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_gui_app` call
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:NonRTTestApp,JUCE_PROJECT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:NonRTTestApp,JUCE_VERSION>")

target_link_libraries(NonRTTestApp PRIVATE
    juce::juce_gui_extra
    spl_plot
    test_utils)
