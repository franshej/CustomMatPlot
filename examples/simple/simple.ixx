module;

#include <cmp_plot.h>
#include <juce_gui_extra/juce_gui_extra.h>

export module simple;

export class simple : public juce::Component {
  cmp::Plot m_plot;

 public:
  simple() : m_plot{cmp::Plot()} {
    setSize(1200, 800);
    addAndMakeVisible(m_plot);
    m_plot.plot({{1, 3, 7, 9, 13}});
  };

  /** @internal */
  void resized() override { m_plot.setBounds(getBounds()); };
};