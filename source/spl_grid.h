#pragma once

#include "spl_grid.h"
#include <juce_gui_basics/juce_gui_basics.h>

struct BaseGrid : juce::Component {
public:
  BaseGrid() : m_grid_colour(juce::Colours::dimgrey) {}
  BaseGrid(const juce::Colour grid_colour) : m_grid_colour(grid_colour) {}

  ~BaseGrid() = default;

  void setGraphBounds(const juce::Rectangle<int>& graph_area);
  void resized() override;
  void paint(juce::Graphics &g) override;

private:
  virtual void createGrid() = 0;

protected:
  std::vector<juce::Path> m_grid_path;
  juce::Rectangle<int> m_graph_area;
  juce::Colour m_grid_colour;
};

struct Grid : BaseGrid {
  void createGrid() override;
};
