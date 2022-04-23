/**
 * Copyright (c) 2022 Frans Rosencrantz
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

/**
 * @file cmp_trace.h
 *
 * @brief Componenet for drawing tracepoints.
 *
 * @ingroup CustomMatPlotInternal
 *
 * @author Frans Rosencrantz
 * Contact: Frans.Rosencrantz@gmail.com
 *
 */

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <cmp_datamodels.h>

#include <optional>

namespace cmp {

/** @brief The label corner that is located at the tracepoint center.  */
enum class TraceLabelCornerPosition {
  top_left,
  top_right,
  bottom_left,
  bottom_right
};

/** @brief A struct that defines a tracepoint. */
template <class ValueType>
struct TracePoint : public juce::Component {
#if THREE_WAY_COMP
  /** Spaceship */
  constexpr bool operator<=>(const TracePoint<ValueType>& rhs) {
    return this->getPosition() <=> rhs.getPosition();
  }
#else
  /** No spaceship  :( */
  constexpr bool operator==(const TracePoint<ValueType>& rhs) {
    return this->getPosition() == rhs.getPosition();
  }
#endif

  /** Compare with other data point. */
  constexpr bool operator==(const juce::Point<ValueType>& other_data_point) {
    return m_data_point == other_data_point;
  }

  /** Set the data point. return true if succeeded */
  bool setDataPoint(const juce::Point<ValueType>& new_data_point);

  /** @brief This lambda is triggered when a the data value is changed.
   *
   * @param trace_point pointer
   * @param previous_data_point previous data point.
   * @param new_data_point the new data point.
   */
  std::function<void(juce::Component* trace_point,
                     const juce::Point<float> previous_data_point,
                     const juce::Point<float> new_data_point)>
      onDataValueChanged = nullptr;

  /** @internal */
  void resized() override;
  /** @internal */
  void paint(juce::Graphics& g) override;
  /** @internal */
  void lookAndFeelChanged() override;

  /** @internal */
  juce::LookAndFeel* m_lookandfeel;

  /** The x and y values of the tracepoint. */
  juce::Point<ValueType> m_data_point;
};

/** @brief A struct that defines a tracepoint using floats. */
typedef TracePoint<float> TracePoint_f;

/** @brief A struct that defines a trace label. */
template <class ValueType>
struct TraceLabel : public juce::Component {
  /** Set the graph labels from point. */
  void setGraphLabelFrom(const juce::Point<ValueType>& graph_value,
                         const CommonPlotParameterView common_plot_params);

  /** @internal */
  void resized() override;
  /** @internal */
  void paint(juce::Graphics& g) override;
  /** @internal */
  void lookAndFeelChanged() override;

  /** The x and y labels. */
  cmp::Label m_x_label, m_y_label;

  /** Defines which label corner that is located at the tracepoint center. */
  TraceLabelCornerPosition trace_label_corner_pos{
      TraceLabelCornerPosition::top_left};

 private:
  /** @internal */
  juce::LookAndFeel* m_lookandfeel;
};

/** @brief A typedef defines a tracelabel using floats. */
typedef TraceLabel<float> TraceLabel_f;

/** @brief A struct that defines a tracelabel and a tracepoint */
template <class ValueType>
struct TraceLabelPoint {
  std::unique_ptr<TraceLabel<ValueType>> trace_label;

  std::unique_ptr<TracePoint<ValueType>> trace_point;

  const GraphLine* associated_graph_line{nullptr};
};

/** @brief A struct that defines a tracelabel and a tracepoint using floats. */
typedef TraceLabelPoint<float> TraceLabelPoint_f;

/**
 * \class Trace
 * \brief A class for drawing tracepoints
 *
 * The idea is to use this class to display the x, y value of a one more
 * points on one or more graphs.
 */
class Trace {
 public:
  /** Destructor. Setting lookandfeel to nullptr. */
  ~Trace();

  /** @brief Clear all tracepoints.
   *
   * Removes all tracepoints.
   *
   * @return void.
   */
  void clear() noexcept;

  /** @brief Get the associated GraphLine.
   *
   * @param TracePoint juce::Componenet* a TracePoint component.
   * @return GraphLine* the associated GraphLine if found else nullptr
   */
  const GraphLine* getAssociatedGraphLine(
      const juce::Component* trace_point) const;

  /** @brief Get the graph position for a tracepoint or trace label.
   *
   * @param trace_point_or_label either a tracepoint or tracelabel component.
   * @return the graph position.
   */
  juce::Point<float> getGraphPosition(
      const juce::Component* trace_point_label) const;

  /** @brief Add or remove a tracepoint.
   *
   * The tracepoint is removed if it's already exists. Must be followed by
   * calling 'updateTracePointsBoundsFrom' to display the trace-point.
   * For efficiency, it's recomended to envoke 'updateTracePointsBoundsFrom'
   * once after several trace-points has been added.
   *
   * @param trace_point_coordinate the coordinate where the point will be drawn.
   * @param graph_line pointer to a graphline.
   * @return void.
   */
  void addOrRemoveTracePoint(const juce::Point<float>& trace_point_coordinate,
                             const GraphLine* graph_line);

  /** @brief Update the tracepoint bounds from graph attributes.
   *
   * @param common_plot_params common plot parameters.
   * @return void.
   */
  void updateTracePointsBoundsFrom(
      const CommonPlotParameterView common_plot_params);

  /** @brief Update a single tracepoint bounds from graph attributes.
   *
   * @param trace_label_or_point either a tracepoint or tracelabel.
   * @param common_plot_params common plot parameters.
   * @return void.
   */
  void updateSingleTracePointBoundsFrom(
      juce::Component* trace_label_or_point,
      const CommonPlotParameterView common_plot_params);

  /** @brief Add the tracepoints to a parent component
   *
   * @param parent_comp the component that the tracepoint will be added to as
   * a child component.
   * @return void.
   */
  void addAndMakeVisibleTo(juce::Component* parent_comp);

  /** @brief Set the lookandfeel for the tracepoints and tracelabels.
   *
   * @param lnf pointer to the lookandfeel instance.
   * @return void.
   */
  void setLookAndFeel(juce::LookAndFeel* lnf);

  /** @brief Set the data value for a tracepoint.
   *
   * @param tracepoint the tracepoint which data value will be set.
   * @param common_plot_params common plot parameters.
   * @param new_position the new position for the tracepoint.
   * @return true if the value was changed.
   */
  bool setDataValueFor(juce::Component* trace_point,
                       const juce::Point<float>& new_position,
                       const CommonPlotParameterView common_plot_params);

  /** @brief Set the coner position of a single tracelabel.
   *
   * @param trace_point the associated tracepoint.
   * @param mouse_position the position of the mouse.
   * @return true if the corner position was changed.
   */
  bool setCornerPositionForLabelAssociatedWith(
      juce::Component* trace_point, const juce::Point<int>& mouse_position);

  /** @brief Check if a juce::Component* is one of the added tracepoints.
   *
   * @param component a juce component
   * @return bool true if the component is one of the tracepoints.
   */
  bool isComponentTracePoint(const juce::Component* component) const;

  /** @brief Check if a juce::Component* is one of the added tracelabels.
   *
   * @param component a juce component
   * @return bool true if the component is one of the tracelabels.
   */
  bool isComponentTraceLabel(const juce::Component* component) const;

  /** @brief Update tracepoints on a graph_line changed y-data
   *
   * If a graph_line has changed y-data you can use this function to update the
   * tracepoints associated with it.
   *
   * @param graph_line the tracepoint associated with this graph_line will be
   * updated.
   * @return void.
   */
  void updateTracePointsAssociatedWith(const GraphLine* graph_line);

  /** @brief This lambda is triggered when a tracepoint value is changed.
   *
   * @param trace_point pointer
   * @param previous_data_point previous data value.
   * @param new_data_point the new data value.
   */
  std::function<void(const juce::Component* trace_point,
                     const juce::Point<float> previous_data_point,
                     const juce::Point<float> new_data_point)>
      onTracePointChanged = nullptr;

 private:
  /** @internal */
  void tracePointCbHelper(const juce::Component* trace_point,
                          const juce::Point<float> previous_data_point,
                          const juce::Point<float> new_data_point);

  /** @internal */
  void addSingleTracePointAndLabel(
      const juce::Point<float>& trace_point_coordinate,
      const GraphLine* graph_line);

  /** @internal */
  void removeSingleTracePointAndLabel(
      const juce::Point<float>& trace_point_coordinate);

  /** @internal */
  void updateSingleTraceLabelTextsAndBoundsInternal(
      TraceLabelPoint_f* trace_point_label,
      const CommonPlotParameterView common_plot_params,
      bool force_corner_position = false);

  /** @internal */
  void updateTracePointsLookAndFeel();

  /** @internal */
  std::vector<TraceLabelPoint_f>::const_iterator
  findTraceLabelPointIteratorFrom(const juce::Component* trace_point) const;

  juce::LookAndFeel* m_lookandfeel;

  std::vector<TraceLabelPoint_f> m_trace_labelpoints;
};

}  // namespace cmp