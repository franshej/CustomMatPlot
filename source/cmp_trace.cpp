/**
 * Copyright (c) 2022 Frans Rosencrantz
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "cmp_trace.h"

#include "cmp_graph_line.h"
#include "cmp_plot.h"

namespace cmp {

static TraceLabelCornerPosition getCornerPosition(
    juce::Point<int> trace_position, juce::Point<int> center_position) {
  const auto dxdy = trace_position - center_position;

  const auto is_x_pos = dxdy.getX() > 0;
  const auto is_y_pos = dxdy.getY() > 0;

  auto trace_label_corner_pos = TraceLabelCornerPosition::top_left;

  if (is_x_pos && is_y_pos)
    trace_label_corner_pos = TraceLabelCornerPosition::top_left;
  else if (is_x_pos && !is_y_pos)
    trace_label_corner_pos = TraceLabelCornerPosition::bottom_left;
  else if (!is_x_pos && is_y_pos)
    trace_label_corner_pos = TraceLabelCornerPosition::top_right;
  else if (!is_x_pos && !is_y_pos)
    trace_label_corner_pos = TraceLabelCornerPosition::bottom_right;

  return trace_label_corner_pos;
}

template <class ValueType>
bool TracePoint<ValueType>::setDataPoint(
    const juce::Point<ValueType>& new_data_point) {
  if (m_data_point != new_data_point) {
    if (onDataValueChanged) {
      onDataValueChanged(this, m_data_point, new_data_point);
    }

    m_data_point = new_data_point;

    return true;
  }
  return false;
}

template <class ValueType>
void TraceLabel<ValueType>::setGraphLabelFrom(
    const juce::Point<ValueType>& graph_value,
    const CommonPlotParameterView common_plot_params) {
  if (m_lookandfeel) {
    auto lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);

    m_x_label.first =
        "X: " +
        valueToString(graph_value.getX(), common_plot_params, true).first;
    m_y_label.first =
        "Y: " +
        valueToString(graph_value.getY(), common_plot_params, false).first;

    const auto [x_label_bounds, y_label_bounds] =
        lnf->getTraceXYLabelBounds(m_x_label.first, m_y_label.first);

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
    lnf->drawTraceLabel(g, m_x_label, m_y_label, getLocalBounds());
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

const GraphLine* Trace::getAssociatedGraphLine(
    const juce::Component* trace_point) const {
  const auto it = findTraceLabelPointIteratorFrom(trace_point);
  if (it == m_trace_labelpoints.end())
    return nullptr;
  else
    return it->associated_graph_line;
}

juce::Point<float> Trace::getGraphPosition(
    const juce::Component* trace_point_label) const {
  const auto tlp_it = findTraceLabelPointIteratorFrom(trace_point_label);

  if (tlp_it != m_trace_labelpoints.end())
    return tlp_it->trace_point->m_data_point;

  return juce::Point<float>();
}

void Trace::addOrRemoveTracePoint(
    const juce::Point<float>& data_point,
    const GraphLine* graph_line) {
  if (std::find_if(m_trace_labelpoints.begin(), m_trace_labelpoints.end(),
                   [&data_point](const auto& tl) {
                     return (*tl.trace_point) == data_point;
                   }) == m_trace_labelpoints.end()) {
    addSingleTracePointAndLabel(data_point, graph_line);
  } else {
    removeSingleTracePointAndLabel(data_point);
  }
}

void Trace::updateTracePointsBoundsFrom(
    const CommonPlotParameterView common_plot_params) {
  for (auto& tlp : m_trace_labelpoints) {
    updateSingleTraceLabelTextsAndBoundsInternal(&tlp, common_plot_params);
  }
}

void Trace::updateSingleTracePointBoundsFrom(
    juce::Component* trace_label_or_point,
    const CommonPlotParameterView common_plot_params) {
  const auto tlp_it = findTraceLabelPointIteratorFrom(trace_label_or_point);

  if (tlp_it == m_trace_labelpoints.end()) {
    return;
  }

  auto it = m_trace_labelpoints.erase(
      tlp_it, tlp_it);  // remove const for ::const_iterator

  updateSingleTraceLabelTextsAndBoundsInternal(&(*it), common_plot_params,
                                               true);
}

void Trace::addAndMakeVisibleTo(juce::Component* parent_comp) {
  for (const auto& tlp : m_trace_labelpoints) {
    parent_comp->addAndMakeVisible(tlp.trace_label.get());
    parent_comp->addAndMakeVisible(tlp.trace_point.get());
  }
}

void Trace::clear() noexcept { m_trace_labelpoints.clear(); }

void Trace::setLookAndFeel(juce::LookAndFeel* lnf) {
  m_lookandfeel = lnf;
  updateTracePointsLookAndFeel();
}

bool Trace::setDataValueFor(juce::Component* trace_point,
                            const juce::Point<float>& new_position,
                            const CommonPlotParameterView common_plot_params) {
  const auto tlp_it = findTraceLabelPointIteratorFrom(trace_point);
  auto it = m_trace_labelpoints.erase(
      tlp_it, tlp_it);  // remove const for ::const_iterator

  if (it != m_trace_labelpoints.end() &&
      it->trace_point->setDataPoint(new_position)) {
    updateSingleTraceLabelTextsAndBoundsInternal(&(*it), common_plot_params);

    return true;
  }

  return false;
}

bool Trace::setCornerPositionForLabelAssociatedWith(
    juce::Component* trace_point, const juce::Point<int>& mouse_position) {
  const auto tlp_it = findTraceLabelPointIteratorFrom(trace_point);

  auto it = m_trace_labelpoints.erase(
      tlp_it, tlp_it);  // remove const for ::const_iterator

  auto trace_label_corner_pos = getCornerPosition(
      mouse_position, it->trace_point->getBounds().getPosition());

  if (it->trace_label->trace_label_corner_pos != trace_label_corner_pos) {
    it->trace_label->trace_label_corner_pos = trace_label_corner_pos;
    return true;
  }

  return false;
}

bool Trace::isComponentTracePoint(const juce::Component* component) const {
  return dynamic_cast<const TracePoint_f*>(component) &&
         findTraceLabelPointIteratorFrom(component) !=
             m_trace_labelpoints.end();
}

bool Trace::isComponentTraceLabel(const juce::Component* component) const {
  return dynamic_cast<const TraceLabel_f*>(component) &&
         findTraceLabelPointIteratorFrom(component) !=
             m_trace_labelpoints.end();
}

void Trace::updateTracePointsAssociatedWith(const GraphLine* graph_line) {
  for (auto& tlp : m_trace_labelpoints) {
    if (tlp.associated_graph_line == graph_line) {
      const auto data_value = tlp.trace_point->m_data_point;

      const auto nearest_data_value =
          graph_line->findClosestDataPointTo(data_value, true);

      tlp.trace_point->setDataPoint(nearest_data_value);
    }
  }
}

void Trace::tracePointCbHelper(const juce::Component* trace_point,
                               const juce::Point<float> previous_data_point,
                               const juce::Point<float> new_data_point) {
  if (onTracePointChanged) {
    onTracePointChanged(trace_point, previous_data_point, new_data_point);
  }
}

void Trace::addSingleTracePointAndLabel(const juce::Point<float>& data_point,
                                        const GraphLine* graph_line) {
  auto trace_label = std::make_unique<TraceLabel_f>();
  auto trace_point = std::make_unique<TracePoint_f>();

  if (m_lookandfeel) trace_label->setLookAndFeel(m_lookandfeel);
  if (m_lookandfeel) trace_point->setLookAndFeel(m_lookandfeel);

  trace_point->onDataValueChanged = [this](const auto* trace_point,
                                           const auto prev_data,
                                           const auto new_data) {
    this->tracePointCbHelper(trace_point, prev_data, new_data);
  };

  trace_point->setDataPoint(data_point);

  m_trace_labelpoints.push_back(
      {move(trace_label), move(trace_point), graph_line});
}

void Trace::removeSingleTracePointAndLabel(
    const juce::Point<float>& data_point) {
  m_trace_labelpoints.erase(
      std::remove_if(m_trace_labelpoints.begin(), m_trace_labelpoints.end(),
                     [&data_point](const auto& tlp) {
                       return *tlp.trace_point == data_point;
                     }));
}

void Trace::updateSingleTraceLabelTextsAndBoundsInternal(
    TraceLabelPoint_f* tlp, const CommonPlotParameterView common_plot_params,
    bool force_corner_position) {
  if (m_lookandfeel) {
    auto lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);

    const auto data_value = tlp->trace_point->m_data_point;
    tlp->trace_label->setGraphLabelFrom(data_value, common_plot_params);

    const auto x_bound = tlp->trace_label->m_x_label.second;
    const auto y_bound = tlp->trace_label->m_y_label.second;

    const auto local_trace_bounds =
        lnf->getTraceLabelLocalBounds(x_bound, y_bound);
    auto trace_bounds = local_trace_bounds;

    const auto trace_position =
        lnf->getTracePointPositionFrom(common_plot_params, data_value) +
        common_plot_params.graph_bounds.getPosition();

    auto trace_label_corner_pos =
        force_corner_position
            ? tlp->trace_label->trace_label_corner_pos
            : getCornerPosition(common_plot_params.graph_bounds.getCentre(),
                                trace_position);

    switch (trace_label_corner_pos) {
      case TraceLabelCornerPosition::top_left:
        trace_bounds.setPosition(trace_position);
        break;
      case TraceLabelCornerPosition::top_right:
        trace_bounds.setPosition(
            trace_position.getX() - trace_bounds.getWidth(),
            trace_position.getY());
        break;
      case TraceLabelCornerPosition::bottom_left:
        trace_bounds.setPosition(
            trace_position.getX(),
            trace_position.getY() - trace_bounds.getHeight());
        break;
      case TraceLabelCornerPosition::bottom_right:
        trace_bounds.setPosition(
            trace_position.getX() - trace_bounds.getWidth(),
            trace_position.getY() - trace_bounds.getHeight());
        break;
      default:
        break;
    }

    tlp->trace_label->setBounds(trace_bounds);

    auto trace_point_bounds = lnf->getTracePointLocalBounds();
    trace_point_bounds.setCentre(trace_position);
    tlp->trace_point->setBounds(trace_point_bounds);
  }
}

void Trace::updateTracePointsLookAndFeel() {
  for (auto& trace_label_point : m_trace_labelpoints) {
    trace_label_point.trace_label->setLookAndFeel(m_lookandfeel);
    trace_label_point.trace_point->setLookAndFeel(m_lookandfeel);
  }
}

std::vector<TraceLabelPoint_f>::const_iterator
Trace::findTraceLabelPointIteratorFrom(
    const juce::Component* trace_point_or_label) const {
  return std::find_if(m_trace_labelpoints.begin(), m_trace_labelpoints.end(),
                      [&trace_point_or_label](const auto& tpl) {
                        return tpl.trace_point.get() == trace_point_or_label ||
                               tpl.trace_label.get() == trace_point_or_label;
                      });
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

}  // namespace cmp
