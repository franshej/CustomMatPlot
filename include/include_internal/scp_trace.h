/**
 * @file scp_trace.h
 *
 * @brief Componenet for drawing trace points.
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

/** @brief A struct that defines a view of a position. */
struct PositionView {
  /** Constructors. */
  PositionView(const juce::Point<int>& position) : position_view(position){};
  PositionView(const juce::Point<int>&& position) = delete;

  /** Default spaceship. */
  constexpr auto operator<=>(const PositionView&) const = default;

  /** The x and y position view of the trace point. */
  const juce::Point<int>& position_view;
};

/** @brief A struct that defines a trace point. */
template <class ValueType>
struct TracePoint : public PositionView, public juce::Component {
  TracePoint();

  using PositionView::operator<=>;

  /** @internal */
  void resized() override;
  /** @internal */
  void paint(juce::Graphics& g) override;
  /** @internal */
  void lookAndFeelChanged() override;
};

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

  /** The x and y values of the trace point. */
  juce::Point<ValueType> m_graph_values;

  /** @internal */
  juce::LookAndFeel* m_lookandfeel;
};

/** @brief A struct that defines a tracelabel using floats. */
typedef TraceLabel<float> TraceLabel_f;

/**
 * \class Trace
 * \brief A class for drawing trace points
 *
 * The idea is to use this class to display the x, y value of a one more
 * points on a graph.
 */
class Trace {
 public:
  ~Trace();

  /** @brief Add or remove a trace point.
   *
   * Add a trace point that will be drawn. The point is
   * removed if it's already exists.
   *
   * @param trace_point the trace point that will be drawn.
   * @return void.
   */
  void addOrRemoveTracePoint(const juce::Point<float>& trace_point);

  /** @brief Update the trace point bounds from plot attributes.
   *
   * @param plot_attributes common plot attributes.
   * @return void.
   */
  void updateTracePointBoundsFrom(const GraphAttributesView& plot_attributes);

  /** @brief Add the trace points to a parent component
   *
   * @param parent_comp the component that the trace point will be added to as
   * a child component.
   * @return void.
   */
  void addAndMakeVisibleTo(juce::Component* parent_comp);

  void setLookAndFeel(juce::LookAndFeel* lnf);

 private:
  void updateTracePointsLookAndFeel();

  juce::LookAndFeel* m_lookandfeel;
  std::vector<std::unique_ptr<TraceLabel_f>> m_trace_label;
};

}  // namespace scp
