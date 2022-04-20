/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <cmp_plot.h>
#include <juce_gui_extra/juce_gui_extra.h>

namespace examples {
class label : public juce::Component {
  // Declare plot object.
  cmp::Plot m_plot;

 public:
  label() : m_plot{cmp::Plot()} {
    setSize(1200, 800);

    // Add the plot object as a child component.
    addAndMakeVisible(m_plot);

    // Title label
    m_plot.setTitle("Plot title.");

    // X-axis label
    m_plot.setXLabel("X axis label.");

    // Y-axis label
    m_plot.setYLabel("Y axis label.");

    // Plot some values.
    m_plot.plot({{1, 3, 7, 9, 13}});
  };

  void resized() override {
    // Set the bounds of the plot to fill the whole window.
    m_plot.setBounds(getBounds());
  };
};
}  // namespace examples
