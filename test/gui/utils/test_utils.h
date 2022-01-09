#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

static scp::Plot* getPlotFromID(
    std::map<std::string, std::unique_ptr<scp::Plot>>& plot_holder,
    const int id) {
  if (id > plot_holder.size()) return nullptr;

  auto it = plot_holder.begin();

  for (int i = 1; i < plot_holder.size() + 1; ++i) {
    if (id == i) break;
    it++;
  }

  return it->second.get();
}

static juce::Rectangle<int> getScreenArea() {
  return juce::Desktop::getInstance().getDisplays().getMainDisplay().userArea;
}

struct node {
  std::function<void(juce::Component*, const std::string&)> fun_ptr = nullptr;
  std::string name;
  std::shared_ptr<node> next = nullptr;
};