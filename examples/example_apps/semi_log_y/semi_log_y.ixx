module;

#include <cmp_plot.h>
#include <example_utils.h>
#include <juce_gui_extra/juce_gui_extra.h>

export module semi_log_y;

namespace examples {
export class semi_log_y : public juce::Component {
  // Declare plot object.
  cmp::SemiLogY m_plot;

 public:
  semi_log_y() : m_plot{cmp::SemiLogY()} {
    setSize(1200, 800);

    // Add the plot object as a child component.
    addAndMakeVisible(m_plot);

    const auto values = {
        cmp::generateUniformRandomVector<float>(1024, 1.0f, 10'000.0f),
        cmp::generateUniformRandomVector<float>(1024, 0.001f, 0.1f)};

    // Plot some values.
    m_plot.plot(values);
  };

  void resized() override {
    // Set the bounds of the plot to fill the whole window.
    m_plot.setBounds(getBounds());
  };
};
}  // namespace examples
