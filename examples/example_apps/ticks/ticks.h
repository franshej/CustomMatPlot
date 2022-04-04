#include <cmp_plot.h>
#include <juce_gui_extra/juce_gui_extra.h>

namespace examples {
class ticks : public juce::Component {
  // Declare plot object.
  cmp::Plot m_plot;

 public:
  ticks() : m_plot{cmp::Plot()} {
    setSize(1200, 800);

    // Add the plot object as a child component.
    addAndMakeVisible(m_plot);

    // Plot some values.
    m_plot.plot({{1, 3, 7, 9, 13}});

    // Set ticks and tick-labals.
    m_plot.setXTicks({1.7f, 2.0f, 3.23f});
    m_plot.setXTickLabels({"Blabla", "asd", "asd�f"});

    m_plot.setYTicks({2.6f, 5.9f, 12.23f});
    m_plot.setYTickLabels({"Hej", "Po", "Daj"});
  };

  void resized() override {
    // Set the bounds of the plot to fill the whole window.
    m_plot.setBounds(getBounds());
  };
};
}  // namespace examples
