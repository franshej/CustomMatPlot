/**
 * @file spl_grid.h
 *
 * @brief Componenets for creating grids and grid labels
 *
 * @ingroup SimpleCustomPlot
 *
 * @author Frans Rosencrantz
 * Contact: Frans.Rosencrantz@gmail.com
 *
 */

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "spl_graph_line.h"

typedef std::vector<std::unique_ptr<GraphLine>> GridLines;

enum scaling { linear = 0, logarithmic = 1 };

/**
 * Base class implementation of grid component
 *
 * Componenet for creating grids and grid lables. The idea is to create the
 * grids behind the actual graph(s) together with graph labels outside the graph
 * area. It can alsa be used to only create the lables without the grids.
 *
 */

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

  /** @brief Set the bounds of where the grids will be drawn
   *
   *  The graph area must be within the bounds of this componenet. The
   *  grid lables will be draw with one 'font_size' outside the graph area.
   *
   *  @param graph_area The area of where the grids will be drawn
   *  @return void.
   */
  void setGraphBounds(const juce::Rectangle<int> &graph_area);

  /** @brief Set the Y-limits
   *
   *  The first horizontal grid line will be drawn at min and the last grid line
   *  at max. 
   *
   *  @param min minimum value to draw the grid line/lable
   *  @param max maximum value to draw the grid line/lable
   *  @return void.
   */
  void setYLim(const float min, const float max);

  /** @brief Set the X-limits
   *
   *  The first vertical grid line will be drawn at min and the last grid line
   *  at max. Labels will be set according to min/max values.
   *
   *  @param min minimum value to draw the grid line/label
   *  @param max maximum value to draw the grid line/label
   *  @return void.
   */
  void setXLim(const float min, const float max);

  /** @brief Display grids
   *
   *  Grids will be shown if grid_on is set to true. Grid lables will be shown
   *  in either case.
   *
   *  @param grid_on set to true to show grids
   *  @return void.
   */
  void setGridON(const bool grid_on);

  void resized() override;
  void paint(juce::Graphics &g) override;

 private:
  /** @brief Clear and reserve the vectors containing the actual grids
   *
   *  The idea is to use this function to clear and reserve the data holders
   *  containing the grids before they are being populated.
   *
   *  @param vertical_grid_lines vertical grids to be cleared and reserved.
   *  @param horizontal_grid_lines horizontal grids to be cleared and reserved.
   *  @return void.
   */
  virtual void prepareDataHolders(GridLines &vertical_grid_lines,
                                  GridLines &horizontal_grid_lines) = 0;
  /** @brief Construct the grid
   *
   *  This function is pure virtual and must be overwritten. This function
   *  should preferably use 'addGridLineVertical' and 'addGridLineHorizontal' to
   *  add the grid lines.
   *
   *  @return void.
   */
  virtual void createGrid(std::vector<float> &x_positions,
                          std::vector<float> &y_positions,
                          scaling &vertical_scaling,
                          scaling &horizontal_scaling) = 0;

  void setLabels(const std::function<float(const float)> xToXPos,
                 const std::function<float(const float)> yToYPos);

  GridLines m_vertical_grid_lines, m_horizontal_grid_lines;

 protected:
  /** @brief Add a vertical grid line
   *
   *  Add an vertical grid line at a x-position. graph_type can either be
   *  logarithmic or linear.
   *
   *  @return void.
   */
  template <class graph_type>
  void addGridLineVertical(const float x_val);

  template <class graph_type>
  void addGridLineHorizontal(const float y_val);

  std::vector<juce::Path> m_grid_path;
  juce::Rectangle<int> m_graph_area;
  juce::Colour m_grid_colour, m_text_colour, m_frame_colour;
  std::pair<float, float> m_limX, m_limY;
  std::vector<std::pair<std::string, juce::Rectangle<int>>> m_y_axis_labels,
      m_x_axis_labels;

  bool m_is_grid_on;
  float m_font_size;
};

/*============================================================================*/

struct Grid : BaseGrid {
  void createGrid(std::vector<float> &x_positions,
                  std::vector<float> &y_positions, scaling &vertical_scaling,
                  scaling &horizontal_scaling) override;

  void prepareDataHolders(GridLines &vertical_grid_lines,
                          GridLines &horizontal_grid_lines) override;

 private:
  unsigned m_num_vertical_lines, m_num_horizontal_lines;
};

/*============================================================================*/

struct SemiLogXGrid : BaseGrid {
  void createGrid(std::vector<float> &x_positions,
                  std::vector<float> &y_positions, scaling &vertical_scaling,
                  scaling &horizontal_scaling) override;
  void prepareDataHolders(GridLines &vertical_grid_lines,
                          GridLines &horizontal_grid_lines) override;

 private:
  float m_min_exp, m_max_exp, m_exp_diff;
  int m_num_lines_exp;
  unsigned m_num_vertical_lines, m_num_horizontal_lines;
};
