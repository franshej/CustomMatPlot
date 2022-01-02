/**
 * @file scp_trace.h
 *
 * @brief Componenet for drawing tracepoints.
 *
 * @ingroup SimpleCustomPlotInternal
 *
 * @author Frans Rosencrantz
 * Contact: Frans.Rosencrantz@gmail.com
 *
 */

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <scp_datamodels.h>

namespace scp {

/** @brief A struct that defines a tracepoint. */
template <class ValueType>
struct TracePoint : public juce::Component {
  /** Spaceship */
  constexpr bool operator<=>(const TracePoint<ValueType>& rhs) {
    return this->getPosition() <=> rhs.getPosition();
  }

  /** @internal */
  void resized() override;
  /** @internal */
  void paint(juce::Graphics& g) override;
  /** @internal */
  void lookAndFeelChanged() override;

  /** @internal */
  juce::LookAndFeel* m_lookandfeel;
};

/** @brief A struct that defines a tracepoint using floats. */
typedef TracePoint<float> TracePoint_f;

/** @brief A struct that defines a trace label. */
template <class ValueType>
struct TraceLabel : public juce::Component {
  constexpr bool operator==(const juce::Point<ValueType>& other_graph_values) {
    return m_graph_values == other_graph_values;
  }

  void setGraphValue(const juce::Point<ValueType>& graph_value);

  /** @internal */
  void resized() override;
  /** @internal */
  void paint(juce::Graphics& g) override;
  /** @internal */
  void lookAndFeelChanged() override;

  /** The x and y labels. */
  scp::Label m_x_label, m_y_label;

  /** The x and y values of the tracepoint. */
  juce::Point<ValueType> m_graph_values;

  /** @internal */
  juce::LookAndFeel* m_lookandfeel;
};

/** @brief A struct that defines a tracelabel using floats. */
typedef TraceLabel<float> TraceLabel_f;

/**
 * \class Trace
 * \brief A class for drawing tracepoints
 *
 * The idea is to use this class to display the x, y value of a one more
 * points on a graph.
 */
class Trace {
 public:
  ~Trace();

  /** @brief Add or remove a tracepoint.
   *
   * The tracepoint is removed if it's already exists.
   *
   * @param trace_point_coordinate the coordinate where the point will be drawn.
   * @return void.
   */
  void addOrRemoveTracePoint(const juce::Point<float>& trace_point_coordinate);

  /** @brief Update the tracepoint bounds from graph attributes.
   *
   * @param graph_attributes common graph attributes.
   * @return void.
   */
  void updateTracePointBoundsFrom(const GraphAttributesView& graph_attributes);

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

 private:
  /** @internal */
  void addSingleTracePointAndLabel(
      const juce::Point<float>& trace_point_coordinate);
  /** @internal */
  void removeSingleTracePointAndLabel(
      const juce::Point<float>& trace_point_coordinate);
  /** @internal */
  void updateTracePointsLookAndFeel();
  /** @internal */
  juce::LookAndFeel* m_lookandfeel;
  /** @internal */
  std::vector<std::unique_ptr<TraceLabel_f>> m_trace_labels;
  /** @internal */
  std::vector<std::unique_ptr<TracePoint_f>> m_trace_points;
};

}  // namespace scp
