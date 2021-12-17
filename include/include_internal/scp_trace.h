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

struct BaseTracePoint {
  /** Default spaceship. */
  constexpr auto operator<=>(const BaseTracePoint&) const = default;

  /** The x and y coordinate of the trace point. */
  juce::Point<int> coordinate;
};

/** @brief A struct that defines a trace point. */
template <class ValueType>
struct TracePoint : public BaseTracePoint, public juce::Component {
  using BaseTracePoint::operator<=>;

  constexpr bool operator==(
      const juce::Point<ValueType>& other_graph_value) const {
    return graph_value == other_graph_value;
  }

  void resized() override{};
  void paint(juce::Graphics& g) override{};
  void lookAndFeelChanged() override{};

  /** The x and y text values of the trace point. */
  std::string x_text, y_text;

  /** The x and y values of the trace point. */
  juce::Point<ValueType> graph_value;
};

/** @brief A struct that defines a trace point using floats. */
typedef TracePoint<float> TracePoint_f;

/**
 * \class Trace
 * \brief A class for drawing trace points
 *
 * The idea is to use this component to display the x,y value of a one more
 * points on a graph.
 */
class Trace : public juce::Component {
 public:
  /** @brief Add or remove a trace point.
   *
   *  Add a trace point that will be drawn. The point is
   *  removed if it's already exists.
   *
   *  @param TracePoint_f reference the trace point that will be drawn.
   *  @return void.
   */
  void addOrRemoveTracePoint(const juce::Point<float>& trace_point);

  /** @internal */
  void resized() override;
  /** @internal */
  void paint(juce::Graphics& g) override;
  /** @internal */
  void lookAndFeelChanged() override;

 private:
  juce::LookAndFeel* m_lookandfeel = nullptr;
  std::vector<std::unique_ptr<TracePoint_f>> m_trace_points;
};

}  // namespace scp
