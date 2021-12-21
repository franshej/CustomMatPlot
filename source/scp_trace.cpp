#include "scp_trace.h"

#include "spl_plot.h"

namespace scp {
template <class ValueType>
void TracePoint<ValueType>::setGraphValue(
    const juce::Point<ValueType>& graph_value) {
  if (m_lookandfeel) {
    auto lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);
    m_graph_value = graph_value;
    m_x_label.first =
        "X: " + convertFloatToString<float>(m_graph_value.getX(), 2, 6);
    m_y_label.first =
        "Y: " + convertFloatToString<float>(m_graph_value.getY(), 2, 6);

    const auto [x_label_bounds, y_label_bounds] =
        lnf->getTraceXYLabelBounds(m_x_label.first, m_x_label.first);

    m_x_label.second = x_label_bounds;
    m_y_label.second = y_label_bounds;
  }
}

template <class ValueType>
void TracePoint<ValueType>::resized() {}

template <class ValueType>
void TracePoint<ValueType>::paint(juce::Graphics& g) {
  if (m_lookandfeel) {
    auto lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);
    lnf->drawTracePoint(g, m_x_label, m_y_label);
  }
}

template <class ValueType>
void TracePoint<ValueType>::lookAndFeelChanged() {
  if (auto* lnf = dynamic_cast<Plot::LookAndFeelMethods*>(&getLookAndFeel())) {
    m_lookandfeel = lnf;
  } else {
    m_lookandfeel = nullptr;
  }
}

void Trace::addOrRemoveTracePoint(const juce::Point<float>& trace_point,
                                  juce::Component* parent_comp) {
  if (std::find_if(m_trace_points.begin(), m_trace_points.end(),
                   [&trace_point](const auto& tp) {
                     return (*tp) == trace_point;
                   }) == m_trace_points.end()) {
    auto tp = std::make_unique<TracePoint_f>();
    tp->setGraphValue(trace_point);
    m_trace_points.emplace_back(move(tp));
    parent_comp->addAndMakeVisible(m_trace_points.back().get());
  } else {
    std::remove_if(
        m_trace_points.begin(), m_trace_points.end(),
        [&trace_point](const auto& tp) { return (*tp) == trace_point; });
  }
}

void Trace::setLookAndFeel(juce::LookAndFeel* lnf) {
  if (lnf) m_lookandfeel = lnf;
  updateTracePointsLookAndFeel();
}

void Trace::updateTracePointsLookAndFeel() {
  if (m_lookandfeel) {
    for (auto& trace_point : m_trace_points) {
      trace_point->setLookAndFeel(m_lookandfeel);
    }
  }
}

}  // namespace scp
