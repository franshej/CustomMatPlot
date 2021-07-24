#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "spl_graph_line.h"
#include "spl_grid.h"

struct BaseGrid : juce::Component {
 public:
  BaseGrid(const juce::Colour grid_colour = juce::Colours::dimgrey,
           const juce::Colour text_colour = juce::Colours::white,
           const juce::Colour frame_colour = juce::Colours::white)
      : m_grid_colour(grid_colour),
        m_text_colour(text_colour),
        m_frame_colour(frame_colour),
        m_limX({0, 0}),
        m_limY({0, 0}),
        m_is_grid_on(false),
        m_font_size(16.f) {}

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
  virtual void resizeDataHolders() = 0;
  virtual void calculateNumHorizontalVerticalLines() = 0;

 protected:
  std::pair<unsigned, unsigned> getNumHorizontalVerticalLinesLog();

  void setLabels(const std::function<float(const float)> xToXPos,
                 const std::function<float(const float)> yToYPos);

  template<class graph_type>
  void addGridLineVertical(const float x_val);

  template<class graph_type>
  void addGridLineHorizontal(const float y_val);

  std::vector<std::unique_ptr<GraphLine>> m_vertical_grid_lines,
      m_horizontal_grid_lines;

  std::vector<juce::Path> m_grid_path;
  juce::Rectangle<int> m_graph_area;
  juce::Colour m_grid_colour, m_text_colour, m_frame_colour;
  std::pair<float, float> m_limX, m_limY;
  std::vector<std::pair<std::string, juce::Rectangle<int>>> m_y_axis_labels,
      m_x_axis_labels;

  bool m_is_grid_on;
  float m_font_size;
  int m_num_vertical_lines, m_num_horizontal_lines, m_num_tot_lines;
};

struct Grid : BaseGrid {
  void createGrid() override;
  void resizeDataHolders() override;
  void calculateNumHorizontalVerticalLines() override;
};

struct SemiLogXGrid : BaseGrid {
  void createGrid() override;
  void resizeDataHolders() override;
  void calculateNumHorizontalVerticalLines() override;

 private:
  float m_min_exp, m_max_exp, m_exp_diff;
  int m_num_lines_exp;
};
