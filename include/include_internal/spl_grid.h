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

#include "scp_datamodels.h"
#include "spl_utils.h";

namespace scp {

typedef uint32_t Scaling;

/**
 * A Parameter struct
 * Containing graphic parameters for text front and grid, text and frame colour.
 */
struct GridGraphicParams {
  /**
   * The colour of the grids that will be drawn.
   */
  scp::ParamVal<juce::Colour> grid_colour;
  /**
   * The colour of the grid labels.
   */
  scp::ParamVal<juce::Colour> label_colour;
  /**
   * The colour of the grid labels.
   */
  scp::ParamVal<juce::Colour> frame_colour;
  /**
   * The font of the grid labels
   */
  scp::ParamVal<juce::Font> label_font;
};

/**
 * A Parameter struct
 * Containing parameters for grid bounds and limits of the grids.
 */
struct GridConfigParams {
  /**
   * X and Y limitation of the grid.
   * First grid is drawn at lim.min and the last grid is drawn at lim.max
   */
  scp::ParamVal<scp::Lim_f> x_lim, y_lim;

  /**
   * The bounds of where the grids will be drawn.
   */
  scp::ParamVal<juce::Rectangle<int>> grid_area;

  /**
   * Set to true if grid should be visable.
   */
  scp::ParamVal<bool> grid_on;

  /**
   * Set to true if tiny grids should be used.
   */
  scp::ParamVal<bool> tiny_grid_on;
};

/**
 * Base class implementation of grid component
 *
 * Componenet for creating grids and grid labels. The idea with this componenet
 * is to create the grids behind the actual graph(s) together with graph labels
 * outside the graph area. It can also be used to only create the grid labels
 * without the grids. Default are grids off.
 *
 */

class BaseGrid : public juce::Component {
 public:
  BaseGrid(const GridGraphicParams& params);
  BaseGrid();
  ~BaseGrid() = default;

  /** @brief Set the graphic parameters
   *
   *  Change one or more of the graphic parameters. It's possible to only change
   *  one parameter in the struct without overriding the other parameters.
   *  Example:
   *  GridGraphicParams params;
   *
   *  params.label_font = getJuceFont();
   *
   *  // Only the parameter label_font will be overwritten.
   *  setGraphicParams(params);
   *
   *  @param params parameter struct
   *  @return void.
   */
  void setGraphicParams(GridGraphicParams& params) {}

  /** @brief Set the bounds of where the grids will be drawn
   *
   *  The grid area must be within the bounds of this componenet. The
   *  grid labels will be draw with a half 'font_size' outside the grid area.
   *
   *  @param grid_area The area of where the grids will be drawn
   *  @return void.
   */
  void setGridBounds(const juce::Rectangle<int>& grid_area);

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
   *  @param tiny_grid_on set to true to show tiny grids
   *  @return void.
   */
  void setGridON(const bool grid_on, const bool tiny_grids_on);

  /** @brief Override the x-ticks
   *
   *  Ticks are the markers denoting data points on axes.
   *
   *  @param x_labels x-labels to be shown.
   *  @return void.
   */
  void setXTicks(const std::vector<float>& x_ticks);

  /** @brief Override the x-labels
   *
   *  Override the auto generated x-labels.
   *
   *  @param x_labels x-labels to be shown.
   *  @return void.
   */
  void setXLabels(const std::vector<std::string>& x_labels);

  /** @brief Override the y-labels
   *
   *  Override the auto generated x-labels.
   *
   *  @param y_labels y-labels to be shown.
   *  @return void.
   */
  void setYLabels(const std::vector<std::string>& y_labels);

  /** @brief Override the y-ticks
   *
   *  Ticks are the markers denoting data points on axes.
   *
   *  @param y_labels y-labels to be shown.
   *  @return void.
   */
  void setYTicks(const std::vector<float>& y_ticks);

  void resized() override;
  void paint(juce::Graphics& g) override;
  void lookAndFeelChanged() override;

 private:
  /** @brief Populate the x and y ticks
   *
   *  The idea is to use this function to populate the x_ticks & y_ticks.
   *
   *  @param x_ticks x-ticks to be populated.
   *  @param y_ticks y-ticks to be populated.
   *  @param vertical_scaling the scaling of the vertical axis.
   *  @param horizontal_scaling the scaling of the horizontal axis.
   *  @return void.
   */
  void createAutoGridTicks(std::vector<float>& x_ticks,
                           std::vector<float>& y_ticks,
                           Scaling vertical_scaling,
                           Scaling horizontal_scaling);

  virtual void setScaling(Scaling& vertical_scaling,
                          Scaling& horizontal_scaling) noexcept = 0;

  void createLabels();

  template <class graph_type>
  void addGridLineVertical(const float x_val);

  template <class graph_type>
  void addGridLineHorizontal(const float y_val);

  GridGraphicParams createDefaultGraphicParams() const;

  GridLines m_vertical_grid_lines, m_horizontal_grid_lines;
  std::vector<float> m_custom_x_ticks, m_custom_y_ticks;
  std::vector<std::string> m_custom_x_labels, m_custom_y_labels;
  std::vector<juce::Path> m_grid_path;

  GridGraphicParams m_graphic_params;
  std::unique_ptr<scp::FrameComponent> m_frame;

 protected:
  LookAndFeelMethodsBase* m_lookandfeel;
  GridConfigParams m_config_params;

  std::vector<std::pair<std::string, juce::Rectangle<int>>> m_y_axis_labels,
      m_x_axis_labels;
};

/*============================================================================*/

/**
 * Component to create X and Y grids and grid labels
 *
 * The idea with this componenet when both the x and y axis are scaled linear.
 *
 */

struct Grid : BaseGrid {
 public:
  using BaseGrid::BaseGrid;

 private:
  void setScaling(Scaling& vertical_scaling,
                  Scaling& horizontal_scaling) noexcept override;
};

/*============================================================================*/

/**
 * Component to create X and Y grids and grid labels
 *
 * The idea with this componenet when both the x axis is scaled logrithmic and y
 * axis is linear.
 *
 */

struct SemiLogXGrid : BaseGrid {
 public:
  using BaseGrid::BaseGrid;

 private:
  void setScaling(Scaling& vertical_scaling,
                  Scaling& horizontal_scaling) noexcept override;
};

}  // namespace scp