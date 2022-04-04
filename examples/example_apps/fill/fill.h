#include <cmp_plot.h>
#include <example_utils.h>
#include <juce_gui_extra/juce_gui_extra.h>

namespace examples {
class fill : public juce::Component {
  // Declare plot object.
  cmp::Plot m_plot;

 public:
  fill() : m_plot{cmp::Plot()} {
    setSize(1200, 800);

    // Add the plot object as a child component.
    addAndMakeVisible(m_plot);

    // Plot some values.
    m_plot.plot({cmp::generateSineWaveVector<float>(1024, -10.0f, 0.0f, 10),
                 cmp::generateSineWaveVector<float>(1024, 10.0f, 20.0f, 2)});

    // Fill the area between the two first graph lines with colour aliceblue
    // with an alpha of 0.5.
    m_plot.fillBetween({{0, 1}}, {juce::Colours::aliceblue.withAlpha(0.3f)});
  };

  void resized() override {
    // Set the bounds of the plot to fill the whole window.
    m_plot.setBounds(getBounds());
  };
};
}  // namespace examples
