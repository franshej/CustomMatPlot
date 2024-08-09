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

  /** @brief Find closest point on graph from pixel point.
   *
   * @param this_pixel_point the point on the graph.
   * @param check_only_distance_from_x check the absolut distance if false else
   * only the x distance.
   * @return {closest point on graph, closest data point value, data point
   * index}
   */
  std::tuple<juce::Point<float>, juce::Point<float>, size_t>
  findClosestPixelPointTo(const juce::Point<float>& this_pixel_point,
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

  /** @brief Get data point for a pixel point index.
   *
   *  @param pixel_point_index the pixel point index.
   *  @return the data point.
   */
  juce::Point<float> getDataPointFromPixelPointIndex(
      size_t pixel_point_index) const;

  /** @brief Get data point for a pixel point index.
   *
   *  @param data_point_index the pixel point index.
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
  const std::vector<float>& getYData() const noexcept;

  /** @brief Get x-values
   *
   *  Get a const reference of the x-values.
   *
   *  @return a const reference of the x-values.
   */
  const std::vector<float>& getXData() const noexcept;

  /** @brief Get the pixel points
   *
   *  Get a const reference of the calculated pixel points.
   *
   *  @return const reference of the calculated pixel points.
   */
  const PixelPoints& getPixelPoints() const noexcept;

  /* @brief Get the pixel point indices
   *
   *  Get a const reference of the calculated pixel point indices.
   *
   *  @return const reference of the calculated pixel point indices.
   */
  const std::vector<size_t>& getPixelPointIndices() const noexcept;

  /** @brief Set the colour of graph
   *
   *  Set the colour of graph.
   *
   *  @param graph_colour the colour of the graph
   *  @return void.
   */
  void setColour(const juce::Colour graph_colour);

  /** @brief Update the pixel points indices x-value in the pixel points.
   *
   *  This function updates the pixel points if any new parameter is set. Should
   *  be called after an parameter changed to update the graph. However, call
   *  updateXYPixelPoints() if the min/max x/y-limits are move equally.
   *
   *  @param update_only_these_indices only update these indices.
   *  @return void.
   */
  void updateXIndicesAndPixelPoints(
      const std::vector<size_t>& update_only_these_indices = {});

  /** @brief Update the pixel points indices and y-value in the pixel points.
   *
   *  This function updates the pixel points if any new parameter is set. Should
   *  be called after an parameter changed to update the graph. However, call
   *  updateXYPixelPoints() if the min/max x/y-limits are move equally.
   *
   *  @param update_only_these_indices only update these indices.
   *  @return void.
   */
  void updateYIndicesAndPixelPoints(
      const std::vector<size_t>& update_only_these_indices = {});

  /** @brief move pixel point in graphline
   *
   * @param d_pixel_point the delta pixel point.
   * @param pixel_point_index the pixel point index.
   * @return void.
   */
  void movePixelPoint(const juce::Point<float>& d_pixel_point,
                      size_t pixel_point_index);

  /** @brief Update the y/x-value in the pixel points.
   *
   *  This function updates the pixel points if any new parameter is set.
   *
   *  @note updateXIndicesAndPixelPoints() and updateYIndicesAndPixelPoints()
   *  must have been called anytime before this function.
   *
   *  @param update_only_these_indices only update these indices.
   *  @return void.
   */
  void updateXYPixelPoints();

  /** @brief Set GraphLine type
   *
   * Set the type of GraphLine. @see GraphLineType.
   *
   * @param graph_line_type the type of GraphLine.
   * @return void.
   */
  void setType(const GraphLineType graph_line_type);

  /** @brief Get GraphLine type
   *
   * Get the type of GraphLine. @see GraphLineType.
   *
   * @return the type of GraphLine.
   */
  GraphLineType getType() const noexcept;

  //==============================================================================

  /** @internal */
  void resized() override;
  /** @internal */
  void paint(juce::Graphics& g) override;
  /** @internal */
  void lookAndFeelChanged() override;

 private:
  void updateYIndicesAndPixelPointsIntern(
      const std::vector<size_t>& update_only_these_indices);
  void updateXIndicesAndPixelPointsIntern(
      const std::vector<size_t>& update_only_these_indices);

  std::vector<float> m_x_data, m_y_data;
  std::vector<std::size_t> m_x_based_ds_indices, m_xy_based_ds_indices;
  PixelPoints m_pixel_points;
  GraphLineType m_graph_line_type{GraphLineType::normal};

  const CommonPlotParameterView* m_common_plot_params{nullptr};
  juce::LookAndFeel* m_lookandfeel{nullptr};
  GraphAttribute m_graph_attributes;
};

/**
 *  \struct GraphLineList
 *  \brief a class to hold a list of graph lines.
 */
struct GraphLineList : public std::vector<std::unique_ptr<GraphLine>> {

  /** @brief Get the number of graph lines for a specific type.
   *
   * @tparam t_graph_line_type the type of graph line.
   * @return the number of graph lines.
   */
  template <GraphLineType t_graph_line_type>
  size_t size() const noexcept;

  /** @brief Resize the number of graph lines for a specific type.
   * 
   * @tparam t_graph_line_type the type of graph line.
   * @param new_size the new size of the graph line list.
   * @return void.
   */
  template <GraphLineType t_graph_line_type>
  void resize(size_t new_size);

  /** @brief Set x- or y-limits for vertical and horizontal lines.
   * 
   * Set the y limit for vertical lines and x limit for horizontal lines.
   * 
   * @tparam t_graph_line_type the type of graph line.
   * @param x_or_y_limit x- or y-limit.
   * @return void.
   */
    template <GraphLineType t_graph_line_type, typename ValueType>
    void setLimitsForVerticalOrHorizontalLines(const Lim<ValueType>& x_or_y_limit);

    private:
    /** @internal */
    template <GraphLineType t_graph_line_type>
    std::vector<GraphLine*> getGraphLinesOfType();
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
