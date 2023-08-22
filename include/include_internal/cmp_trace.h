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

#include <cmp_datamodels.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include <iterator>
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
  explicit TracePoint(const size_t data_point_index,
                      const GraphLine* graph_line)
      : data_point_index(data_point_index), associated_graph_line(graph_line){};

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
    return getDataPoint() == other_data_point;
  }

  /** Set the data point. */
  bool setDataPoint(const size_t data_point_index, const GraphLine* graph_line);

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

  /** Get data point.
   * @return the data point.
   */
  juce::Point<ValueType> getDataPoint() const;

  /** @internal */
  void resized() override;
  /** @internal */
  void paint(juce::Graphics& g) override;
  /** @internal */
  void lookAndFeelChanged() override;

  /** @internal */
  juce::LookAndFeel* m_lookandfeel;

  /** The index of the data point that this trace point is associated with. */
  size_t data_point_index{0};

  /** The graph line that this trace point is associated with. */
  const GraphLine* associated_graph_line{nullptr};
};

/** @brief A struct that defines a tracepoint using floats. */
typedef TracePoint<float> TracePoint_f;

/** @brief A struct that defines a trace label. */
template <class ValueType>
struct TraceLabel : public juce::Component {
  /** Set the graph labels from point. */
  void setGraphLabelFrom(const juce::Point<ValueType>& graph_value,
                         const CommonPlotParameterView& common_plot_params);

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
  void updateTraceLabel();
  /** @internal */
  juce::LookAndFeel* m_lookandfeel;
  /** @internal */
  CommonPlotParameterView const* m_common_plot_params{nullptr};
  /** @internal */
  juce::Point<ValueType> const* m_graph_value{nullptr};
};

/** @brief A typedef defines a tracelabel using floats. */
typedef TraceLabel<float> TraceLabel_f;

/** @brief A struct that defines a tracelabel and a tracepoint */
template <class ValueType>
struct TraceLabelPoint {
  /* @brief Constructor.
   *
   * @param trace_label a TraceLabel component.
   * @param trace_point a TracePoint component.
   * @param trace_point_visiblility_type the visibility of the tracepoint.
   */
  TraceLabelPoint(std::unique_ptr<TraceLabel<ValueType>>,
                  std::unique_ptr<TracePoint<ValueType>>,
                  const TracePointVisibilityType);

  /** @brief Set if tracepoint is selected or not.
   *
   * @param selected true if selected.
   * @return void.
   */
  void setSelection(const bool selected);

  /** @brief Check if selected.
   *
   * @return true if selected.
   */
  bool isSelected() const;

  /** @brief Update visibility */
  void updateVisibility();

  std::unique_ptr<TraceLabel<ValueType>> trace_label;
  std::unique_ptr<TracePoint<ValueType>> trace_point;

 private:
  /** @internal */
  bool selected{false};
  /** @internal */
  void updateVisibilityInternal();

  TracePointVisibilityType trace_point_visiblility_type{
      TracePointVisibilityType::visible};
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
  /** Constructor. */
  Trace(CommonPlotParameterView common_plot_params);

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

  /** @brief Get the data position for a tracepoint or trace label.
   *
   * @param trace_point_or_label either a tracepoint or tracelabel component.
   * @return the data position.
   */
  juce::Point<float> getDataPosition(
      const juce::Component* trace_point_label) const;

  /** @brief Add or remove a tracepoint.
   *
   * The tracepoint is removed if it's already exists. Must be followed by
   * calling 'updateTracePointsBounds' to display the trace-point.
   *
   * @param graph_line pointer to a graph line.
   * @param graph_point_index the index of the graph point that is associated
   * with this tracepoint
   * @return void.
   */
  void addOrRemoveTracePoint(
      const GraphLine* graph_line, const size_t graph_point_index,
      const TracePointVisibilityType trace_point_visibility =
          TracePointVisibilityType::visible);

  /** @brief Add single tracepoint.
   *
   * Add tracepoint. Must be followed by
   * calling 'updateTracePointsBounds' to display the trace-point.
   *
   * @param graph_line pointer to a graph line.
   * @param graph_point_index the index of the graph point that is associated
   * with this tracepoint
   * @param trace_point_visibility the visibility of the tracepoint.
   * @return void.
   */
  void addTracePoint(const GraphLine* graph_line,
                     const size_t graph_point_index,
                     const TracePointVisibilityType trace_point_visibility);

  /** @brief Get tracepoint from juce::component.
   *
   * @param trace_point must be a tracepoint component.
   * @return const TracePoint* the tracepoint if found else nullptr.
   */
  const TracePoint<float>* getTracePointFrom(
      const juce::Component* trace_point) const;

  /** @brief Get all tracepoint and tracelabels.
   *
   * @return const std::vector<TraceLabelPoint_f>& a vector of all tracepoints
   * and tracelabels.
   */
  const std::vector<TraceLabelPoint_f>& getTraceLabelPoints() const;

  /** @brief Update the tracepoint bounds.
   *
   * @return void.
   */
  void updateTracePointsBounds();

  /** @brief Update a single tracepoint bounds from graph attributes.
   *
   * @param trace_label_or_point either a tracepoint or tracelabel.
   * @return void.
   */
  void updateSingleTracePointBoundsFrom(juce::Component* trace_label_or_point);

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

  /** @brief Set the graph point for a tracepoint.
   *
   * @param tracepoint the tracepoint which data value will be set.
   * @param graph_point_index the index of the graph point that is associated
   * with this tracepoint
   * @param graph_line the graph line that the graph point belongs to.
   *
   * @return true if the value was changed.
   */
  bool setDataPointFor(juce::Component* trace_point,
                       const size_t graph_point_index,
                       const GraphLine* graph_line);

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

  /** @brief Make a tracepoint selected
   *
   * @param component the tracepoint to be selected.
   * @param selected true if the tracepoint should be selected.
   * @return void.
   */
  void selectTracePoint(const juce::Component* component, const bool selected);

 private:
  /** @internal */
  void tracePointCbHelper(const juce::Component* trace_point,
                          const juce::Point<float> previous_data_point,
                          const juce::Point<float> new_data_point);

  /** @internal */
  void addSingleTracePointAndLabelInternal(
      const GraphLine* graph_line, const size_t graph_point_index,
      const TracePointVisibilityType trace_point_visibility);

  /** @internal */
  void removeSingleTracePointAndLabel(const GraphLine* graph_line,
                                      const size_t graph_point_index);

  /** @internal */
  void updateSingleTraceLabelTextsAndBoundsInternal(
      TraceLabelPoint_f* trace_point_label, bool force_corner_position = false);

  /** @internal */
  void updateTracePointsLookAndFeel();

  /** @internal */
  bool doesTracePointExist(const GraphLine* graph_line,
                           const size_t graph_point_index) const;

  /** @internal */
  std::vector<TraceLabelPoint_f>::const_iterator
  findTraceLabelPointIteratorFrom(const juce::Component* trace_point) const;
  std::vector<TraceLabelPoint_f>::iterator findTraceLabelPointIteratorFrom(
      const juce::Component* trace_point);
  juce::LookAndFeel* m_lookandfeel;
  std::vector<TraceLabelPoint_f> m_trace_labelpoints;
  CommonPlotParameterView m_common_plot_params;
};

}  // namespace cmp