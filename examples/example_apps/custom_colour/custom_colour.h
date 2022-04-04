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

    // Create some data to visulize.
    auto y_data = {cmp::generateUniformRandomVector((1 << 11), 10.0f, 100.0f),
                   cmp::generateSineWaveVector((1 << 11), -3.0f, 14.0f, 3.0f),
                   cmp::generateSineWaveVector((1 << 11), -5.0f, 2.0f, 6.0f)};

    // Setting new colours on graph one and two.
    auto graph_attributes = cmp::GraphAttributeList(y_data.size());
    graph_attributes[0].graph_colour = juce::Colours::pink;
    graph_attributes[1].graph_colour = juce::Colours::blueviolet;

    // Plot some values.
    m_plot.plot(y_data, {}, graph_attributes);
  };

  void resized() override {
    // Set the bounds of the plot to fill the whole window.
    m_plot.setBounds(getBounds());
  };
};
}  // namespace examples
