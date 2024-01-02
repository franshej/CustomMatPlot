/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

/**
 * @file cmp_grid.h
 *
 * @brief Componenets for creating grids and grid labels
 *
 * @ingroup CustomMatPlotInternal
 *
 * @author Frans Rosencrantz
 * Contact: Frans.Rosencrantz@gmail.com
 *
 */

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "cmp_datamodels.h"
#include "cmp_utils.h"

namespace cmp {

/**
 * Grid class implementation of grid component
 *
 * Componenet for creating grids and grid labels. The idea with this componenet
 * is to create the grids behind the actual graph(s) together with graph labels
 * outside the graph area. It can also be used to only create the grid labels
 * without the grids. Default are grids off.
 *
 */
class Grid : public juce::Component {
 public:
  /** @brief Construct a new Grid object.
   *
   *  @param common_plot_parameter_view the common plot parameters.
   */
  Grid(const CommonPlotParameterView& common_plot_parameter_view)
      : m_common_plot_params(&common_plot_parameter_view){};

  /** @brief Set the bounds of where the grids will be drawn
   *
   *  The grid area must be within the bounds of this componenet. The
   *  grid labels will be draw with a half 'font_size' outside the grid area.
   *
   *  @param grid_area The area of where the grids will be drawn
   *  @return void.
   */
  void setGridBounds(const juce::Rectangle<int>& grid_area);

  /** @brief Enables grid or tiny grid
   *
   *  Turn on grids or tiny grids. @see GridType in cmp:datamodels.h.
   *
   *  @param grid_type typ of grid to be drawn.
   *  @return void.
   */
  void setGridType(const GridType grid_type);

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

  /** @brief Update grids and grid labels
   *
   *  This function updates the grid if any new parameter is set. Should be
   *  called after an parameter is set to update the grid.
   *
   *  @param use_cached_grids if true already computed grid will be used.
   *  @return void.
   */
  void updateGrid(const bool use_cached_grids = false);

  /** @brief Get the max width of the x and y-labels
   *
   *  @return pair<int, int> where first is the x width and second is the y
   *  width.
   */
  const std::pair<int, int> getMaxGridLabelWidth() const noexcept;

  /** @brief This lamda is trigged when the length of a gridline exceeds the
   *  margin.
   *
   *  @param Grid pointer to this grid.
   *  @return void.
   */
  std::function<void(Grid*)> onGridLabelLengthChanged = nullptr;

  //==============================================================================
  /** @internal */
  void resized() override;
  /** @internal */
  void paint(juce::Graphics& g) override;
  /** @internal */
  void lookAndFeelChanged() override;

 private:
  /** @brief Populate the x and y ticks
   *
   *  The idea is to use this function to populate the x_ticks & y_ticks.
   *
   *  @param x_ticks x-ticks to be populated.
   *  @param y_ticks y-ticks to be populated.
   *  @return void.
   */
  void createAutoGridTicks(std::vector<float>& x_ticks,
                           std::vector<float>& y_ticks);
  void createLabels();
  void updateGridInternal(const bool use_cached_grids);
  void addGridLines(const std::vector<float>& ticks,
                    const GridLine::Direction direction);
  void addTranslucentGridLines();

  std::vector<GridLine> m_grid_lines;
  std::vector<float> m_custom_x_ticks, m_custom_y_ticks, m_x_prev_ticks,
      m_y_prev_ticks;
  std::vector<std::string> m_custom_x_labels, m_custom_y_labels;
  std::size_t m_max_width_x, m_max_width_y;
  std::size_t m_num_last_x_labels, m_last_num_y_labels;
  std::size_t m_longest_x_axis_label_length_last_cb_triggerd{0};
  std::size_t m_longest_y_axis_label_length_last_cb_triggerd{0};
  std::vector<juce::Path> m_grid_path;
  GridType m_grid_type = GridType::grid_translucent;

  juce::LookAndFeel* m_lookandfeel;
  const CommonPlotParameterView* m_common_plot_params{nullptr};

  std::vector<std::pair<std::string, juce::Rectangle<int>>> m_y_axis_labels,
      m_x_axis_labels;
};
}  // namespace cmp