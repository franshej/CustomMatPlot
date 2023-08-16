/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <cmp_plot.h>
#include <juce_gui_extra/juce_gui_extra.h>

#include "cmp_datamodels.h"

namespace examples {
class move_graph_points : public juce::Component {
  // Declare plot object.
  cmp::Plot m_plot;

 public:
  move_graph_points() : m_plot{cmp::Plot()} {
    setSize(1200, 800);

    // Add the plot object as a child component.
    addAndMakeVisible(m_plot);
    
    m_plot.setMovePointsType(cmp::GraphPointMoveType::horizontal);

    // Plot some values.
    m_plot.plot({{1, 3, 7, 9, 13}});
  };

  void resized() override {
    // Set the bounds of the plot to fill the whole window.
    m_plot.setBounds(getBounds());
  };
};
}  // namespace examples
