#include <cmp_plot.h>
#include <juce_gui_extra/juce_gui_extra.h>

export module simple;

export class simple : public juce::Component {
  cmp::Plot plot;

 public:
  simple() : plot{cmp::Plot()} {
    setSize(1200, 800);
    addAndMakeVisible(plot);
    plot.plot({{1, 3, 7, 9, 13}});
  };

  /** @internal */
  void resized() override { plot.setBounds(getBounds()); };
  /** @internal */
  void paint(juce::Graphics& g) override{};
  /** @internal */
  void lookAndFeelChanged() override{};
};