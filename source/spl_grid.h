#pragma once

#include "spl_grid.h"
#include <juce_gui_basics/juce_gui_basics.h>

struct BaseGrid : juce::Component {
public:
  BaseGrid(const juce::Colour grid_colour = juce::Colours::dimgrey,
           const juce::Colour text_colour = juce::Colours::white)
      : m_grid_colour(grid_colour), m_text_colour(text_colour), m_limX({0, 0}),
        m_limY({0, 0}) {}

  ~BaseGrid() = default;

  void setGraphBounds(const juce::Rectangle<int> &graph_area);

  void yLim(const float &min, const float &max);
  void xLim(const float &min, const float &max);

  void resized() override;
  void paint(juce::Graphics &g) override;

private:
  virtual void createGrid() = 0;

protected:
  std::vector<juce::Path> m_grid_path;
  juce::Rectangle<int> m_graph_area;
  juce::Colour m_grid_colour, m_text_colour;
  std::pair<float, float> m_limX, m_limY;
  std::vector<std::pair<std::string, juce::Rectangle<int>>> m_y_axis_texts, m_x_axis_texts;
  float m_font_size;
};

struct Grid : BaseGrid {
  void createGrid() override;
};
