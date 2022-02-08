module;

#include <cmp_plot.h>
#include <example_utils.h>
#include <juce_gui_extra/juce_gui_extra.h>

export module semi_log_x;

namespace examples {
export class semi_log_x : public juce::Component {
  // Declare plot object.
  cmp::SemiLogX m_plot;

 public:
  semi_log_x() : m_plot{cmp::SemiLogX()} {
    setSize(1200, 800);

    // Add the plot object as a child component.
    addAndMakeVisible(m_plot);

    const auto values = {
        cmp::generateUniformRandomVector<float>(1024, 1.0f, 10'000.0f)};

    // Plot some values.
    m_plot.plot(values);
  };

  void resized() override {
    // Set the bounds of the plot to fill the whole window.
    m_plot.setBounds(getBounds());
  };
};
}  // namespace examples