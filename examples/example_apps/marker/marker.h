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

    // Create some marker.
    cmp::GraphAttributeList gal(4u);

    gal[0].marker = cmp::Marker::Type::Circle;
    gal[1].marker = cmp::Marker::Type::LeftTriangle;
    gal[2].marker = cmp::Marker::Type::Pentagram;
    gal[3].marker = cmp::Marker::Type::Square;

    // Plot some values.
    m_plot.plot({cmp::generateUniformRandomVector(10, -10.0f, 10.0f),
                 cmp::generateUniformRandomVector(10, -10.0f, 10.0f),
                 cmp::generateUniformRandomVector(10, -10.0f, 10.0f),
                 cmp::generateUniformRandomVector(10, -10.0f, 10.0f)},
                {}, gal);
  };

  void resized() override {
    // Set the bounds of the plot to fill the whole window.
    m_plot.setBounds(getBounds());
  };
};
}  // namespace examples
