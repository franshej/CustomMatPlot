function(add_app_example EXAMPLE_NAME)

set(CMAKE_UTILS_PATH ${CMAKE_SOURCE_DIR}/examples/cmake_utils/)

configure_file(${CMAKE_UTILS_PATH}/app_template.cpp.in
               ${EXAMPLE_NAME}_example_app.cpp @ONLY)

juce_add_gui_app(${EXAMPLE_NAME}_example_app
                 PRODUCT_NAME
                 "Example app ${EXAMPLE_NAME}.")

target_sources(${EXAMPLE_NAME}_example_app PRIVATE
    ${EXAMPLE_NAME}_example_app.cpp
    ${EXAMPLE_NAME}.h)

target_include_directories(${EXAMPLE_NAME}_example_app PRIVATE ./)

target_compile_definitions(${EXAMPLE_NAME}_example_app PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:${EXAMPLE_NAME}_example_app,JUCE_PROJECT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:${EXAMPLE_NAME}_example_app,JUCE_VERSION>")

target_link_libraries(${EXAMPLE_NAME}_example_app PRIVATE
    juce::juce_gui_extra
    cmp_plot
    example_utils)

endfunction()