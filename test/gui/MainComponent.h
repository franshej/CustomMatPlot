#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <spl_plot.h>

class MainComponent : public juce::Component {
public:
  MainComponent();

  void paint(juce::Graphics &) override;
  void resized() override;
  std::vector<std::unique_ptr<Plot>> *get_plot_holder() {
    return &m_plot_holder;
  }

private:
  std::vector<std::unique_ptr<Plot>> m_plot_holder;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
