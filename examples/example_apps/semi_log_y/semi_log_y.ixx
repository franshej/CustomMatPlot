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

    // Plot some values.
    m_plot.plot({generateRandomVector<float>(1024, 0.001f, 100.0f),
                 {0.001f, 1'000'00.0f}});
  };

  void resized() override {
    // Set the bounds of the plot to fill the whole window.
    m_plot.setBounds(getBounds());
  };
};
}  // namespace examples
