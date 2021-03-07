#pragma once

#include "spl_grid.h"
#include "spl_graph_line.h"
#include <juce_gui_basics/juce_gui_basics.h>

struct BaseGrid : juce::Component {
public:
  BaseGrid(const juce::Colour grid_colour = juce::Colours::dimgrey,
           const juce::Colour text_colour = juce::Colours::white,
           const juce::Colour frame_colour = juce::Colours::white)
      : m_grid_colour(grid_colour), m_text_colour(text_colour),
        m_frame_colour(frame_colour), m_limX({0, 0}), m_limY({0, 0}),
        m_is_grid_on(false), m_font_size(16.f) {}

  ~BaseGrid() = default;

  void setGraphBounds(const juce::Rectangle<int> &graph_area);

  void yLim(const float &min, const float &max);
  void xLim(const float &min, const float &max);

  void gridON(const bool grid_on) { m_is_grid_on = grid_on; }

  void setFontSize(const float font_size) { m_font_size = font_size; }

  void resized() override;
  void paint(juce::Graphics &g) override;

private:
  virtual void createGrid() = 0;

protected:
  std::pair<unsigned, unsigned> getNumHorizontalVerticalLines();
  std::pair<unsigned, unsigned> getNumHorizontalVerticalLinesLog();

  std::vector<std::unique_ptr<GraphLine>> m_grid_lines;
  std::vector<std::vector<float>> m_x_coordinates, m_y_coordinates;

  std::vector<juce::Path> m_grid_path;
  juce::Rectangle<int> m_graph_area;
  juce::Colour m_grid_colour, m_text_colour, m_frame_colour;
  std::pair<float, float> m_limX, m_limY;
  std::vector<std::pair<std::string, juce::Rectangle<int>>> m_y_axis_texts,
      m_x_axis_texts;
  bool m_is_grid_on;
  float m_font_size;
};

struct Grid : BaseGrid {
  void createGrid() override;
};

struct SemiLogXGrid : BaseGrid {
	void createGrid() override;
};
