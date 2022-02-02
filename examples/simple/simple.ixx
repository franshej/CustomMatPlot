module;

#include <cmp_plot.h>
#include <juce_gui_extra/juce_gui_extra.h>

export module simple;

export class simple : public juce::Component {
  // Declare plot object.
  cmp::Plot m_plot;

 public:
  simple() : m_plot{cmp::Plot()} {
    setSize(1200, 800);

    // Add the plot object as a child component.
    addAndMakeVisible(m_plot);

    // Plot some values.
    m_plot.plot({{1, 3, 7, 9, 13}});
  };

  void resized() override {
    // Set the bounds of the plot to fill the whole window.
    m_plot.setBounds(getBounds());
  };
};