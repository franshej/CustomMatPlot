/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <cmp_plot.h>
#include <example_utils.h>
#include <juce_gui_extra/juce_gui_extra.h>

namespace examples {
class marker : public juce::Component {
  // Declare plot object.
  cmp::Plot m_plot;

 public:
  marker() : m_plot{cmp::Plot()} {
    setSize(1200, 800);

    // Add the plot object as a child component.
    addAndMakeVisible(m_plot);

    // Plot some values, each series with its own marker.
    m_plot.plot(
        {{.y = cmp::generateUniformRandomVector(10, -10.0f, 10.0f),
          .attribute = {.marker = cmp::Marker::Type::Circle}},
         {.y = cmp::generateUniformRandomVector(10, -10.0f, 10.0f),
          .attribute = {.marker = cmp::Marker::Type::LeftTriangle}},
         {.y = cmp::generateUniformRandomVector(10, -10.0f, 10.0f),
          .attribute = {.marker = cmp::Marker::Type::Pentagram}},
         {.y = cmp::generateUniformRandomVector(10, -10.0f, 10.0f),
          .attribute = {.marker = cmp::Marker::Type::Square}}});
  };

  void resized() override {
    // Set the bounds of the plot to fill the whole window.
    m_plot.setBounds(getBounds());
  };
};
}  // namespace examples
