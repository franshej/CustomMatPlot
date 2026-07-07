juce_add_gui_app(plot3d_test_app
    PRODUCT_NAME "example_app_plot3d_test")

target_sources(plot3d_test_app PRIVATE
    plot3d_test_app.cpp)

target_compile_definitions(plot3d_test_app PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:plot3d_test_app,JUCE_PROJECT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:plot3d_test_app,JUCE_VERSION>")

target_link_libraries(plot3d_test_app PRIVATE
    juce::juce_gui_extra
    cmp_plot)
