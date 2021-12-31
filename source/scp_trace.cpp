#include "scp_trace.h"

#include "spl_plot.h"

namespace scp {
template <class ValueType>
void TraceLabel<ValueType>::setGraphValue(
    const juce::Point<ValueType>& graph_values) {
  if (m_lookandfeel) {
    auto lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);
    m_graph_values = graph_values;
    m_x_label.first =
        "X: " + convertFloatToString<float>(m_graph_values.getX(), 2, 6);
    m_y_label.first =
        "Y: " + convertFloatToString<float>(m_graph_values.getY(), 2, 6);

    const auto [x_label_bounds, y_label_bounds] =
        lnf->getTraceXYLabelBounds(m_x_label.first, m_x_label.first);

    m_x_label.second = x_label_bounds;
    m_y_label.second = y_label_bounds;
  }
}

template <class ValueType>
void TraceLabel<ValueType>::resized() {}

template <class ValueType>
void TraceLabel<ValueType>::paint(juce::Graphics& g) {
  if (m_lookandfeel) {
    auto lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);
    lnf->drawTracePoint(g, m_x_label, m_y_label);
  }
}

template <class ValueType>
void TraceLabel<ValueType>::lookAndFeelChanged() {
  if (auto* lnf = dynamic_cast<Plot::LookAndFeelMethods*>(&getLookAndFeel())) {
    m_lookandfeel = lnf;
  } else {
    m_lookandfeel = nullptr;
  }
}

Trace::~Trace() {
  m_lookandfeel = nullptr;
  updateTracePointsLookAndFeel();
}

void Trace::addOrRemoveTracePoint(const juce::Point<float>& trace_point) {
  if (std::find_if(m_trace_label.begin(), m_trace_label.end(),
                   [&trace_point](const auto& tp) {
                     return (*tp) == trace_point;
                   }) == m_trace_label.end()) {
    auto tp = std::make_unique<TraceLabel_f>();
    if (m_lookandfeel) tp->setLookAndFeel(m_lookandfeel);
    tp->setGraphValue(trace_point);
    m_trace_label.emplace_back(move(tp));
  } else {
    m_trace_label.erase(
        std::remove_if(
            m_trace_label.begin(), m_trace_label.end(),
            [&trace_point](const auto& tp) { return (*tp) == trace_point; }),
        m_trace_label.end());
  }
  updateTracePointsLookAndFeel();
}

void Trace::updateTracePointBoundsFrom(
    const GraphAttributesView& plot_attributes) {
  if (m_lookandfeel) {
    auto lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);
    for (auto& trace_point : m_trace_label) {
      const auto x_bound = trace_point->m_x_label.second;
      const auto y_bound = trace_point->m_y_label.second;
      const auto graph_values = trace_point->m_graph_values;

      const auto local_trace_bounds =
          lnf->getTraceLabelLocalBounds(x_bound, y_bound);
      auto trace_bounds = local_trace_bounds;
      const auto trace_position =
          lnf->getTracePointPositionFrom(plot_attributes, graph_values);

      trace_bounds.setPosition(trace_position);
      trace_point->setBounds(trace_bounds);
    }
  }
}

void Trace::addAndMakeVisibleTo(juce::Component* parent_comp) {
  for (const auto& trace_point : m_trace_label) {
    parent_comp->addAndMakeVisible(trace_point.get());
  }
}

void Trace::setLookAndFeel(juce::LookAndFeel* lnf) {
  m_lookandfeel = lnf;
  updateTracePointsLookAndFeel();
}

void Trace::updateTracePointsLookAndFeel() {
  for (auto& trace_point : m_trace_label) {
    trace_point->setLookAndFeel(m_lookandfeel);
  }
}

template <class ValueType>
TracePoint<ValueType>::TracePoint() : PositionView(getPosition()) {}

template <class ValueType>
void TracePoint<ValueType>::resized() {}

template <class ValueType>
void TracePoint<ValueType>::paint(juce::Graphics& g) {}

template <class ValueType>
void TracePoint<ValueType>::lookAndFeelChanged() {
  if (auto* lnf = dynamic_cast<Plot::LookAndFeelMethods*>(&getLookAndFeel())) {
    m_lookandfeel = lnf;
  } else {
    m_lookandfeel = nullptr;
  }
}

}  // namespace scp
