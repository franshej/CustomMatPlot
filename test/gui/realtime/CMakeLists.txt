juce_add_gui_app(RTTestApp
    PRODUCT_NAME "Non real-time test app.")

target_sources(RTTestApp PRIVATE
    rt_test_app.cpp
    rt_tests.cpp
    rt_test_macros.h
    rt_test_handler.h)

target_compile_definitions(RTTestApp PRIVATE
    # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
    JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_gui_app` call
    JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_gui_app` call
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:RTTestApp,JUCE_PROJECT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:RTTestApp,JUCE_VERSION>")

target_link_libraries(RTTestApp PRIVATE
    juce::juce_gui_extra
    spl_plot
    test_utils)
