juce_add_gui_app(GuiTestApp
    PRODUCT_NAME "Gui Test App") 

target_sources(GuiTestApp PRIVATE
    Main.cpp
    MainComponent.cpp)

target_compile_definitions(GuiTestApp PRIVATE
    # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
    JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_gui_app` call
    JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_gui_app` call
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:GuiTestApp,JUCE_PROJECT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:GuiTestApp,JUCE_VERSION>")

target_link_libraries(GuiTestApp PRIVATE
    juce::juce_gui_extra
    spl_plot)
