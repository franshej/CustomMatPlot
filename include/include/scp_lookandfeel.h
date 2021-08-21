#pragma once
#include "spl_plot.h"

namespace scp {
class PlotLookAndFeel : public juce::LookAndFeel_V3,
                        public Plot::LookAndFeelMethods {
 public:
  PlotLookAndFeel() { setDefaultPlotColours(); }

  virtual ~PlotLookAndFeel() override {}

  void setDefaultPlotColours() noexcept override {}

  juce::Rectangle<int> getBounds(juce::Rectangle<int>& bounds) const override {
    auto ret = juce::Rectangle<int>(bounds);
    ret.setX(0);
    ret.setY(0);
    return ret;
  }

  juce::Rectangle<int> getGraphAreaBounds(
      juce::Rectangle<int>& bounds) const override {
    return juce::Rectangle<int>();
  }
};

};  // namespace scp