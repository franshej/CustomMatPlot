module;

#include <cmp_plot.h>
#include <example_utils.h>
#include <juce_gui_extra/juce_gui_extra.h>

export module lim;

namespace examples {
export class lim : public juce::Component {
  // Declare plot object.
  cmp::Plot m_plot;

 public:
  lim() : m_plot{cmp::Plot()} {
    setSize(1200, 800);

    // Add the plot object as a child component.
    addAndMakeVisible(m_plot);

    // Plot some values.
    m_plot.plot({{15, 3, 7, 9, 13}});

    // Set x and y limits
    m_plot.xLim(-1.0f, 3.4f);
    m_plot.yLim(3.14f, 42.0f);
  };

  void resized() override {
    // Set the bounds of the plot to fill the whole window.
    m_plot.setBounds(getBounds());
  };
};
}  // namespace examples
