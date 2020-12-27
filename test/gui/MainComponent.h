#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <spl_plot.h>

class MainComponent : public juce::Component {
public:
  MainComponent();

  void paint(juce::Graphics &) override;
  void resized() override;
  std::vector<std::pair<std::unique_ptr<Plot>, std::string>> *get_plot_holder() {
    return &m_plot_holder;
  }

private:
  std::vector<std::pair<std::unique_ptr<Plot>, std::string>> m_plot_holder;
  Plot* m_current_plot = nullptr;
  juce::ComboBox m_test_menu;
  juce::Label m_menu_label;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
