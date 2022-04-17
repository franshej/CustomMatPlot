/**
 * Copyright (c) 2022 Frans Rosencrantz
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

/**
 * @file cmp_graph_line.h
 *
 * @brief Componenets for drawing graph lines.
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
 *  \class GraphLine
 *  \brief A class component to draw 2-D lines/marker symbols. This is
 *  a subcomponenet to cmp::Plot.
 */
class GraphLine : public juce::Component {
 public:
  /**@brief Find closest point on graph from graph point.
   *
   * @param this_graph_point the point on the graph.
   * @param check_only_distance_from_x check the absolut distance if false else
   * only the x distance.
   * @return {closest point on graph, closest data point value}
   */
  std::pair<juce::Point<float>, juce::Point<float>> findClosestGraphPointTo(
      const juce::Point<float>& this_graph_point,
      bool check_only_distance_from_x = false) const;

  /**@brief Find closest data point to this data point.
   *
   * @param this_data_point the data point.
   * @param check_only_distance_from_x check the absolut distance if false else
   * only the x distance.
   * @param only_visible_data_points find the nearest visible data point if
   * true.
   * @return closest data point value to this_data_point.
   */
  juce::Point<float> findClosestDataPointTo(
      const juce::Point<float>& this_data_point,
      bool check_only_distance_from_x = false,
      bool only_visible_data_points = true) const;

  /** @brief Get the colour of the graph.
   *
   *  @param graph_colour the colour of the graph
   *  @return void.
   */
  juce::Colour getColour() const noexcept;

  /** @brief Set the graph attributes.
   *
   *  @see GraphAttribute.
   *
   *  @param graph_attributes the graph_attributes.
   *  @return void.
   */
  void setGraphAttribute(const GraphAttribute& graph_attribute);

  /** @brief Set the y-values for the graph-line
   *
   *  @param y_values vector of y-values.
   *  @return void.
   */
  void setYValues(const std::vector<float>& y_values);

  /** @brief Set the x-values for the graph-line
   *
   *  @param x_values vector of x-values.
   *  @return void.
   */
  void setXValues(const std::vector<float>& x_values);

  /** @brief Get y-values
   *
   *  Get a const reference of the y-values.
   *
   *  @return a const reference of the y-values.
   */
  const std::vector<float>& getYValues() const noexcept;

  /** @brief Get x-values
   *
   *  Get a const reference of the x-values.
   *
   *  @return a const reference of the x-values.
   */
  const std::vector<float>& getXValues() const noexcept;

  /** @brief Get the graph points
   *
   *  Get a const reference of the calculated graph points.
   *
   *  @return const reference of the calculated graph points.
   */
  const GraphPoints& getGraphPoints() const noexcept;

  /** @brief Set the colour of graph
   *
   *  Set the colour of graph.
   *
   *  @param graph_colour the colour of the graph
   *  @return void.
   */
  void setColour(const juce::Colour graph_colour);

  /** @brief Update the x-value in the graph points.
   *
   *  This function updates the graph points if any new parameter is set. Should
   *  be called after an parameter changed to update the graph.
   *
   *  @param common_plot_params common plot parameters.
   *  @return void.
   */
  void updateXGraphPoints(const CommonPlotParameterView common_plot_params);

  /** @brief Update the y-value in the graph points.
   *
   *  This function updates the graph points if any new parameter is set. Should
   *  be called after an parameter changed to update the graph.
   *
   *  @param common_plot_params common plot parameters.
   *  @return void.
   */
  void updateYGraphPoints(const CommonPlotParameterView common_plot_params);

  //==============================================================================

  /** @internal */
  void resized() override;
  /** @internal */
  void paint(juce::Graphics& g) override;
  /** @internal */
  void lookAndFeelChanged() override;

 private:
  void updateYGraphPointsIntern(
      const CommonPlotParameterView common_plot_params) noexcept;
  void updateXGraphPointsIntern(
      const CommonPlotParameterView common_plot_params) noexcept;

  juce::LookAndFeel* m_lookandfeel{nullptr};

  std::vector<float> m_x_data, m_y_data;
  GraphPoints m_graph_points;
  std::vector<std::size_t> m_x_based_ds_indices, m_xy_based_ds_indices;

  GraphAttribute m_graph_attributes;
};

/**
 *  \struct GraphSpread
 *  \brief a class to fill the area between two graph lines.
 */
struct GraphSpread : public juce::Component {
  /**@brief Set the upper and lower bounds of the graph spread to be drawn.
   *
   * @param upper_bound graph_line that decribes the upper boundary.
   * @param lower_bound graph_line that decribes the lower boundary.
   */
  GraphSpread(const GraphLine* upper_bound, const GraphLine* lower_bound,
              const juce::Colour spread_colour)
      : m_upper_bound{upper_bound},
        m_lower_bound{lower_bound},
        m_spread_colour{spread_colour} {}

  const GraphLine *m_upper_bound, *m_lower_bound;
  juce::Colour m_spread_colour;

 private:
  juce::LookAndFeel* m_lookandfeel{nullptr};

  /** @internal */
  void resized() override;
  /** @internal */
  void paint(juce::Graphics& g) override;
  /** @internal */
  void lookAndFeelChanged() override;
};

static inline std::recursive_mutex plot_mutex;

}  // namespace cmp
