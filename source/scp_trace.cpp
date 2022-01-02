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
    lnf->drawTraceLabel(g, m_x_label, m_y_label);
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

void Trace::addOrRemoveTracePoint(
    const juce::Point<float>& trace_point_coordinate) {
  if (std::find_if(m_trace_labels.begin(), m_trace_labels.end(),
                   [&trace_point_coordinate](const auto& tl) {
                     return (*tl) == trace_point_coordinate;
                   }) == m_trace_labels.end()) {
    addSingleTracePointAndLabel(trace_point_coordinate);
  } else {
    removeSingleTracePointAndLabel(trace_point_coordinate);
  }
}

void Trace::updateTracePointBoundsFrom(
    const GraphAttributesView& graph_attributes) {
  if (m_lookandfeel) {
    auto lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);

    auto tp_it = m_trace_points.begin();
    for (auto& trace_label : m_trace_labels) {
      const auto x_bound = trace_label->m_x_label.second;
      const auto y_bound = trace_label->m_y_label.second;
      const auto graph_values = trace_label->m_graph_values;

      const auto local_trace_bounds =
          lnf->getTraceLabelLocalBounds(x_bound, y_bound);
      auto trace_bounds = local_trace_bounds;
      const auto trace_position =
          lnf->getTracePointPositionFrom(graph_attributes, graph_values) +
          graph_attributes.graph_bounds.getPosition();

      trace_bounds.setPosition(trace_position);
      trace_label->setBounds(trace_bounds);

      auto trace_point_bounds = lnf->getTracePointLocalBounds();
      trace_point_bounds.setCentre(trace_position);
      if (tp_it != m_trace_points.end())
        (*tp_it++)->setBounds(trace_point_bounds);
    }
  }
}

void Trace::addAndMakeVisibleTo(juce::Component* parent_comp) {
  for (const auto& trace_label : m_trace_labels) {
    parent_comp->addAndMakeVisible(trace_label.get());
  }
  for (const auto& trace_point : m_trace_points) {
    parent_comp->addAndMakeVisible(trace_point.get());
  }
}

void Trace::setLookAndFeel(juce::LookAndFeel* lnf) {
  m_lookandfeel = lnf;
  updateTracePointsLookAndFeel();
}

void Trace::addSingleTracePointAndLabel(
    const juce::Point<float>& trace_point_coordinate) {
  auto trace_label = std::make_unique<TraceLabel_f>();
  auto trace_point = std::make_unique<TracePoint_f>();

  if (m_lookandfeel) trace_label->setLookAndFeel(m_lookandfeel);
  if (m_lookandfeel) trace_point->setLookAndFeel(m_lookandfeel);

  trace_label->setGraphValue(trace_point_coordinate);
  m_trace_labels.emplace_back(move(trace_label));
  m_trace_points.emplace_back(move(trace_point));
}

void Trace::removeSingleTracePointAndLabel(
    const juce::Point<float>& trace_point_coordinate) {
  auto it_p = m_trace_points.begin();
  auto it_l = m_trace_labels.begin();
  while (it_p != m_trace_points.end() && it_l != m_trace_labels.end()) {
    if (*(*it_l) == trace_point_coordinate) break;
    ++it_p;
    ++it_l;
  }

  m_trace_labels.erase(it_l);
  m_trace_points.erase(it_p);
}

void Trace::updateTracePointsLookAndFeel() {
  for (auto& trace_label : m_trace_labels) {
    trace_label->setLookAndFeel(m_lookandfeel);
  }

  for (auto& trace_point : m_trace_points) {
    trace_point->setLookAndFeel(m_lookandfeel);
  }
}

template <class ValueType>
void TracePoint<ValueType>::resized() {}

template <class ValueType>
void TracePoint<ValueType>::paint(juce::Graphics& g) {
  if (m_lookandfeel) {
    auto lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);
    lnf->drawTracePoint(g, getLocalBounds());
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

}  // namespace scp
