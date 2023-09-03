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

#include <cstddef>

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
  /**
   * @brief Construct a new Graph Line object.
   * @param common_plot_parameter_view the common plot parameters.
   */
  GraphLine(const CommonPlotParameterView& common_plot_parameter_view)
      : m_common_plot_params(&common_plot_parameter_view){};

  /** @brief Find closest point on graph from graph point.
   *
   * @param this_graph_point the point on the graph.
   * @param check_only_distance_from_x check the absolut distance if false else
   * only the x distance.
   * @return {closest point on graph, closest data point value, data point
   * index}
   */
  std::tuple<juce::Point<float>, juce::Point<float>, size_t>
  findClosestGraphPointTo(const juce::Point<float>& this_graph_point,
                          bool check_only_distance_from_x = false) const;

  /** @brief Find closest data point to this data point.
   *
   * @param this_data_point the data point.
   * @param check_only_distance_from_x check the absolut distance if false else
   * only the x distance.
   * @param only_visible_data_points find the nearest visible data point if
   * true.
   * @return closest data point value to this_data_point and the data point
   * index.
   */
  std::pair<juce::Point<float>, size_t> findClosestDataPointTo(
      const juce::Point<float>& this_data_point,
      bool check_only_distance_from_x = false,
      bool only_visible_data_points = true) const;

  /** @brief Get data point for a graph point index.
   *
   *  @param graph_point_index the graph point index.
   *  @return the data point.
   */
  juce::Point<float> getDataPointFromGraphPointIndex(
      size_t graph_point_index) const;

  /** @brief Get data point for a graph point index.
   *
   *  @param data_point_index the graph point index.
   *  @return the data point.
   */
  juce::Point<float> getDataPointFromDataPointIndex(
      size_t data_point_index) const;

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

  /** @brief Get the graph attributes.
   *
   *  @see GraphAttribute.
   *
   *  @return the graph_attributes.
   */
  const GraphAttribute& getGraphAttribute() const noexcept;

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

  /** @brief Set a single x/y value for the graph-line
   *
   *  @param juce::Point<float> the x/y value.
   *  @param index the index of the x/y value.
   *  @return boolean if the index is valid.
   */
  bool setXYValue(const juce::Point<float>& xy_value, size_t index);

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

  /* @brief Get the graph point indices
   *
   *  Get a const reference of the calculated graph point indices.
   *
   *  @return const reference of the calculated graph point indices.
   */
  const std::vector<size_t>& getGraphPointIndices() const noexcept;

  /** @brief Set the colour of graph
   *
   *  Set the colour of graph.
   *
   *  @param graph_colour the colour of the graph
   *  @return void.
   */
  void setColour(const juce::Colour graph_colour);

  /** @brief Update the graph points indices x-value in the graph points.
   *
   *  This function updates the graph points if any new parameter is set. Should
   *  be called after an parameter changed to update the graph. However, call
   *  updateXYGraphPoints() if the min/max x/y-limits are move equally.
   *
   *  @param update_only_these_indices only update these indices.
   *  @return void.
   */
  void updateXIndicesAndGraphPoints(
      const std::vector<size_t>& update_only_these_indices = {});

  /** @brief Update the graph points indices and y-value in the graph points.
   *
   *  This function updates the graph points if any new parameter is set. Should
   *  be called after an parameter changed to update the graph. However, call
   *  updateXYGraphPoints() if the min/max x/y-limits are move equally.
   *
   *  @param update_only_these_indices only update these indices.
   *  @return void.
   */
  void updateYIndicesAndGraphPoints(
      const std::vector<size_t>& update_only_these_indices = {});

  /** @brief move graph point in graphline
   *
   * @param d_graph_point the delta graph point.
   * @param graph_point_index the graph point index.
   * @return void.
   */
  void moveGraphPoint(const juce::Point<float>& d_graph_point,
                      size_t graph_point_index);

  /** @brief Update the y/x-value in the graph points.
   *
   *  This function updates the graph points if any new parameter is set.
   *
   *  @note updateXIndicesAndGraphPoints() and updateYIndicesAndGraphPoints()
   *  must have been called anytime before this function.
   *
   *  @param update_only_these_indices only update these indices.
   *  @return void.
   */
  void updateXYGraphPoints();

  //==============================================================================

  /** @internal */
  void resized() override;
  /** @internal */
  void paint(juce::Graphics& g) override;
  /** @internal */
  void lookAndFeelChanged() override;

 private:
  void updateYIndicesAndGraphPointsIntern(
      const std::vector<size_t>& update_only_these_indices);
  void updateXIndicesAndGraphPointsIntern(
      const std::vector<size_t>& update_only_these_indices);

  juce::LookAndFeel* m_lookandfeel{nullptr};

  std::vector<float> m_x_data, m_y_data;
  GraphPoints m_graph_points;
  std::vector<std::size_t> m_x_based_ds_indices, m_xy_based_ds_indices;
  const CommonPlotParameterView* m_common_plot_params{nullptr};

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
