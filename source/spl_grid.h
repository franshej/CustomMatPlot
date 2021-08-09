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

#include "spl_utils.h";

struct GridGraphicParams {
  scp::ParamVal<juce::Colour> grid_colour;
  scp::ParamVal<juce::Colour> text_colour;
  scp::ParamVal<juce::Colour> frame_colour;
  scp::ParamVal<juce::Font> font;
};

struct GridConfigParams {
  scp::ParamVal<scp::Lim_f> x_lim, y_lim;
  scp::ParamVal<juce::Rectangle<int>> graph_area;
  scp::ParamVal<bool> grid_on;
};

/**
 * Base class implementation of grid component
 *
 * Componenet for creating grids and grid labels. The idea is to create the
 * grids behind the actual graph(s) together with graph labels outside the graph
 * area. It can also be used to only create the grid labels without the grids.
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
        m_graph_area({0, 0, 0, 0}),
        m_font(juce::Font("Arial Rounded MT", 16.f, juce::Font::plain)) {}

  ~BaseGrid() = default;

  void setGraphicParams(GridGraphicParams &params) {}

  /** @brief Set the bounds of where the grids will be drawn
   *
   *  The graph area must be within the bounds of this componenet. The
   *  grid labels will be draw with a half 'font_size' outside the graph area.
   *
   *  @param graph_area The area of where the grids will be drawn
   *  @return void.
   */
  void setGraphBounds(const juce::Rectangle<int> &graph_area);

  /** @brief Set the Y-limits
   *
   *  Set the limits of Y-axis.
   *
   *  @param min minimum value
   *  @param max maximum value
   *  @return void.
   */
  void setYLim(const float min, const float max);

  /** @brief Set the X-limits
   *
   *  Set the limits of X-axis.
   *
   *  @param min minimum value
   *  @param max maximum value
   *  @return void.
   */
  void setXLim(const float min, const float max);

  /** @brief Display grids
   *
   *  Grids will be shown if grid_on is set to true. Grid labels will be shown
   *  in either case. Default is false.
   *
   *  @param grid_on set to true to show grids
   *  @return void.
   */
  void setGridON(const bool grid_on);

  /** @brief Override the x-ticks
   *
   *  Ticks are the markers denoting data points on axes.
   *
   *  @param x_labels x-labels to be shown.
   *  @return void.
   */
  void setXTicks(const std::vector<float> &x_ticks);

  /** @brief Override the x-labels
   *
   *  Override the auto generated x-labels.
   *
   *  @param x_labels x-labels to be shown.
   *  @return void.
   */
  void setXLabels(const std::vector<std::string> &x_labels);

  /** @brief Override the y-labels
   *
   *  Override the auto generated x-labels.
   *
   *  @param y_labels y-labels to be shown.
   *  @return void.
   */
  void setYLabels(const std::vector<std::string> &y_labels);

  /** @brief Override the y-ticks
   *
   *  Ticks are the markers denoting data points on axes.
   *
   *  @param y_labels y-labels to be shown.
   *  @return void.
   */
  void setYTicks(const std::vector<float> &y_ticks);

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
  virtual void prepareDataHolders(scp::GridLines &vertical_grid_lines,
                                  scp::GridLines &horizontal_grid_lines) = 0;
  /** @brief Construct the grid
   *
   *  The idea is to use this function to populate the x_ticks & y_ticks and
   *  choose the scaling of the axis.
   *
   *  @param x_ticks x-ticks to be populated.
   *  @param y_ticks y-ticks to be populated.
   *  @param vertical_scaling set the scaling of the vertical axis.
   *  @param horizontal_scaling set the scaling of the horizontal axis.
   *  @return void.
   */
  virtual void createGrid(std::vector<float> &x_ticks,
                          std::vector<float> &y_ticks,
                          scp::scaling &vertical_scaling,
                          scp::scaling &horizontal_scaling) = 0;

  void createLabels(const std::function<float(const float)> xToXPos,
                    const std::function<float(const float)> yToYPos);

  template <class graph_type>
  void addGridLineVertical(const float x_val);

  template <class graph_type>
  void addGridLineHorizontal(const float y_val);

  juce::Colour m_grid_colour, m_text_colour, m_frame_colour;
  scp::GridLines m_vertical_grid_lines, m_horizontal_grid_lines;
  std::vector<float> m_custom_x_ticks, m_custom_y_ticks;
  std::vector<std::string> m_custom_x_labels, m_custom_y_labels;
  std::vector<juce::Path> m_grid_path;
  juce::Font m_font;

 protected:
  juce::Rectangle<int> m_graph_area;
  std::pair<float, float> m_limX, m_limY;
  std::vector<std::pair<std::string, juce::Rectangle<int>>> m_y_axis_labels,
      m_x_axis_labels;

  bool m_is_grid_on;
};

/*============================================================================*/

struct Grid : BaseGrid {
  void createGrid(std::vector<float> &x_positions,
                  std::vector<float> &y_positions,
                  scp::scaling &vertical_scaling,
                  scp::scaling &horizontal_scaling) override;

  void prepareDataHolders(scp::GridLines &vertical_grid_lines,
                          scp::GridLines &horizontal_grid_lines) override;

 private:
  unsigned m_num_vertical_lines, m_num_horizontal_lines;
};

/*============================================================================*/

struct SemiLogXGrid : BaseGrid {
  void createGrid(std::vector<float> &x_positions,
                  std::vector<float> &y_positions,
                  scp::scaling &vertical_scaling,
                  scp::scaling &horizontal_scaling) override;
  void prepareDataHolders(scp::GridLines &vertical_grid_lines,
                          scp::GridLines &horizontal_grid_lines) override;

 private:
  float m_min_exp, m_max_exp, m_exp_diff;
  int m_num_lines_exp;
  unsigned m_num_vertical_lines, m_num_horizontal_lines;
};
