#include <cmp_plot.h>
#include <juce_gui_extra/juce_gui_extra.h>

export class simple;

class simple : public juce::Component {
 public:
  /** @internal */
  void resized() override{};
  /** @internal */
  void paint(juce::Graphics& g) override{};
  /** @internal */
  void lookAndFeelChanged() override{};
};