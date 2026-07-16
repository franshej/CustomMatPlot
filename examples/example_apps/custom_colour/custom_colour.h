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
class custom_colour : public juce::Component {
  // Declare plot object.
  cmp::Plot m_plot;

 public:
  custom_colour() : m_plot{cmp::Plot()} {
    setSize(1200, 800);

    // Add the plot object as a child component.
    addAndMakeVisible(m_plot);

    // Plot three series, giving the first two a custom colour. Each series
    // carries its own attribute right next to its data.
    m_plot.plot(
        {{.y = cmp::generateUniformRandomVector((1 << 11), 10.0f, 100.0f),
          .attribute = {.series_colour = juce::Colours::pink}},
         {.y = cmp::generateSineWaveVector((1 << 11), -3.0f, 14.0f, 3.0f),
          .attribute = {.series_colour = juce::Colours::blueviolet}},
         {.y = cmp::generateSineWaveVector((1 << 11), -5.0f, 2.0f, 6.0f)}});
  };

  void resized() override {
    // Set the bounds of the plot to fill the whole window.
    m_plot.setBounds(getBounds());
  };
};
}  // namespace examples
