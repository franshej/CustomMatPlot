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
class custom_series_attributes : public juce::Component {
  // Declare plot object.
  cmp::Plot m_plot;

 public:
  custom_series_attributes() : m_plot{cmp::Plot()} {
    setSize(1200, 800);

    // Add the plot object as a child component.
    addAndMakeVisible(m_plot);

    const auto length = (1 << 11);
    constexpr auto length_1st_series = 10;

    // An x-ramp spanning 1..length for a series of the given size.
    auto x_ramp = [length](const std::size_t size) {
      auto v = std::vector<float>(size);
      cmp::iota_delta(v.begin(), v.end(), 1.0f, float(length) / float(size));
      return v;
    };

    // Plot three series, each with its own x, y and attributes.
    m_plot.plot(
        {{.x = x_ramp(length_1st_series),
          .y = cmp::generateSineWaveVector(length_1st_series, -17.0f, 14.0f,
                                           1.0f),
          .attribute = {.series_colour = juce::Colours::pink,
                        .series_opacity = 0.0f,
                        .marker = cmp::Marker::Type::Pentagram}},
         {.x = x_ramp(length),
          .y = cmp::generateSineWaveVector(length, -5.0f, 6.0f, 3.0f),
          .attribute = {.series_colour = juce::Colours::blueviolet,
                        .path_stroke_type = juce::PathStrokeType(10.0f)}},
         {.x = x_ramp(length),
          .y = cmp::generateSineWaveVector(length, -5.0f, 2.0f, 6.0f),
          .attribute = {.dashed_lengths = {{10.0f, 20.0f, 10.0f}}}}});
  };

  void resized() override {
    // Set the bounds of the plot to fill the whole window.
    m_plot.setBounds(getBounds());
  };
};
}  // namespace examples
