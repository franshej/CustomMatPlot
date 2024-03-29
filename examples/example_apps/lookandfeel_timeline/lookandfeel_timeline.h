/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "cmp_datamodels.h"
#include <cmp_extras.hpp>
#include <cmp_plot.h>
#include <juce_gui_extra/juce_gui_extra.h>

using namespace cmp;

namespace examples {
class lookandfeel_timeline : public juce::Component {
  // Declare plot object.
  cmp::Plot m_plot;

  cmp::PlotLookAndFeelTimeline m_look_and_feel;

 public:
  lookandfeel_timeline() : m_plot{cmp::Plot()} {
    setSize(1200, 200);

    // Set the look and feel of the plot object.
    m_plot.setLookAndFeel(&m_look_and_feel);

    // Add the plot object as a child component.
    addAndMakeVisible(m_plot);

    // Plot some values.
    m_plot.plot({{0, 3, 7, 9, 2.5}},{{-100, 20, 40, 50, 220}});

    // Setting x/y limes.
    m_plot.xLim(-10.f, 200.f);
    m_plot.yLim(-10.f, 10.f);

    m_plot.setGridType(GridType::grid_translucent);
  };

  ~lookandfeel_timeline() override {
    // Set the look and feel of the plot object to nullptr.
    m_plot.setLookAndFeel(nullptr);
  };

  void resized() override {
    // Set the bounds of the plot to fill the whole window.
    m_plot.setBounds(getBounds());
  };
};
}  // namespace examples
