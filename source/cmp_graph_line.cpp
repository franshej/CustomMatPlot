/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "cmp_graph_line.h"

#include <cstddef>
#include <mutex>
#include <numeric>
#include <stdexcept>

#include "cmp_downsampler.h"
#include "cmp_plot.h"

namespace cmp {

GraphLineDataView::GraphLineDataView(
    const std::vector<float>& _x_data, const std::vector<float>& _y_data,
    const GraphPoints& _graph_points,
    const std::vector<std::size_t>& _graph_point_indices,
    const GraphAttribute& _graph_attribute)
    : x_data(_x_data),
      y_data(_y_data),
      graph_points(_graph_points),
      graph_point_indices(_graph_point_indices),
      graph_attribute(_graph_attribute) {}

GraphLineDataView::GraphLineDataView(const GraphLine& graph_line)
    : x_data(graph_line.getXValues()),
      y_data(graph_line.getYValues()),
      graph_points(graph_line.getGraphPoints()),
      graph_point_indices(graph_line.getGraphPointIndices()),
      graph_attribute(graph_line.getGraphAttribute()) {}

const GraphAttribute& GraphLine::getGraphAttribute() const noexcept {
  return m_graph_attributes;
}

void GraphLine::setColour(const juce::Colour graph_colour) {
  m_graph_attributes.graph_colour = graph_colour;
}

std::tuple<juce::Point<float>, juce::Point<float>, size_t>
GraphLine::findClosestGraphPointTo(const juce::Point<float>& this_graph_point,
                                   bool check_only_distance_from_x) const {
  // No graph points.
  jassert(!m_graph_points.empty());

  auto closest_graph_point = juce::Point<float>();
  auto closest_data_point = juce::Point<float>();
  auto closest_i = 0u;

  auto closest_distance = std::numeric_limits<float>::max();
  std::size_t i = 0u;
  for (const auto& graph_point : m_graph_points) {
    const auto current_distance =
        check_only_distance_from_x
            ? std::abs(graph_point.getX() - this_graph_point.getX())
            : graph_point.getDistanceSquaredFrom(this_graph_point);
    if (current_distance < closest_distance) {
      closest_distance = current_distance;
      closest_graph_point = graph_point;
      closest_i = i;
      closest_data_point =
          juce::Point<float>(m_x_data[m_xy_based_ds_indices[i]],
                             m_y_data[m_xy_based_ds_indices[i]]);
    }
    i++;
  }

  return {closest_graph_point, closest_data_point,
          m_xy_based_ds_indices[closest_i]};
}

std::pair<juce::Point<float>, size_t> GraphLine::findClosestDataPointTo(
    const juce::Point<float>& this_data_point, bool check_only_distance_from_x,
    bool only_visible_data_points) const {
  // No y_data empty.
  jassert(!m_x_data.empty());

  // x_data & y_data must have the same size
  jassert(m_x_data.size() == m_y_data.size());

  const decltype(m_x_based_ds_indices)* indices = &m_x_based_ds_indices;
  decltype(m_x_based_ds_indices) all_indices;

  if (!only_visible_data_points) {
    all_indices.resize(m_x_data.size());

    std::iota(all_indices.begin(), all_indices.end(), 0u);

    indices = &all_indices;
  }

  auto nearest_x_dist = std::numeric_limits<float>::max();
  std::size_t nearest_i = 0u;

  for (const auto i : *indices) {
    const auto x = m_x_data[i];
    const auto current_x_dist = std::abs(x - this_data_point.getX());

    if (current_x_dist < nearest_x_dist) {
      nearest_x_dist = current_x_dist;
      nearest_i = i;
    }
  }

  const auto closest_data_point =
      juce::Point<float>(m_x_data[nearest_i], m_y_data[nearest_i]);

  return {closest_data_point, nearest_i};
}

juce::Colour GraphLine::getColour() const noexcept {
  return m_graph_attributes.graph_colour.value();
}

juce::Point<float> GraphLine::getDataPointFromGraphPointIndex(
    size_t graph_point_index) const {
  return juce::Point<float>(m_x_data[m_xy_based_ds_indices[graph_point_index]],
                            m_y_data[m_xy_based_ds_indices[graph_point_index]]);
};

juce::Point<float> GraphLine::getDataPointFromDataPointIndex(
    size_t data_point_index) const {
  return juce::Point<float>(m_x_data[data_point_index],
                            m_y_data[data_point_index]);
};

void GraphLine::resized(){};

void GraphLine::paint(juce::Graphics& g) {
  const GraphLineDataView graph_line_data(*this);

  if (m_lookandfeel) {
    const std::lock_guard<std::recursive_mutex> lock(plot_mutex);

    auto lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);
    lnf->drawGraphLine(g, graph_line_data);
  }
}

void GraphLine::lookAndFeelChanged() {
  if (auto* lnf = dynamic_cast<Plot::LookAndFeelMethods*>(&getLookAndFeel())) {
    m_lookandfeel = lnf;
    if (m_common_plot_params) {
      updateXIndicesAndGraphPointsIntern({});
      updateYIndicesAndGraphPointsIntern({});
    }
  } else {
    m_lookandfeel = nullptr;
  }
}

void GraphLine::setGraphAttribute(const GraphAttribute& graph_attribute) {
  if (graph_attribute.dashed_lengths)
    m_graph_attributes.dashed_lengths = graph_attribute.dashed_lengths;

  if (graph_attribute.graph_colour)
    m_graph_attributes.graph_colour = graph_attribute.graph_colour;

  if (graph_attribute.graph_line_opacity)
    m_graph_attributes.graph_line_opacity = graph_attribute.graph_line_opacity;

  if (graph_attribute.on_graph_point_paint)
    m_graph_attributes.on_graph_point_paint =
        graph_attribute.on_graph_point_paint;

  if (graph_attribute.path_stroke_type)
    m_graph_attributes.path_stroke_type = graph_attribute.path_stroke_type;

  if (graph_attribute.marker)
    m_graph_attributes.marker = graph_attribute.marker;
}

void GraphLine::setYValues(const std::vector<float>& y_data) {
  if (m_y_data.size() != y_data.size()) m_y_data.resize(y_data.size());
  std::copy(y_data.begin(), y_data.end(), m_y_data.begin());
}

void GraphLine::setXValues(const std::vector<float>& x_data) {
  if (m_x_data.size() != x_data.size()) m_x_data.resize(x_data.size());
  std::copy(x_data.begin(), x_data.end(), m_x_data.begin());
}

bool GraphLine::setXYValue(const juce::Point<float>& xy_value, size_t index) {
  if (index >= m_x_data.size()) return false;

  m_x_data[index] = xy_value.getX();
  m_y_data[index] = xy_value.getY();

  return true;
}

void GraphLine::moveGraphPoint(const juce::Point<float>& d_graph_point,
                               size_t graph_point_index) {
  if (graph_point_index >= m_x_data.size()) return;

  m_x_data[graph_point_index] += d_graph_point.getX();
  m_y_data[graph_point_index] += d_graph_point.getY();
}

const std::vector<float>& GraphLine::getYValues() const noexcept {
  return m_y_data;
}

const std::vector<float>& GraphLine::getXValues() const noexcept {
  return m_x_data;
}

const GraphPoints& GraphLine::getGraphPoints() const noexcept {
  return m_graph_points;
}

const std::vector<size_t>& GraphLine::getGraphPointIndices() const noexcept {
  return m_xy_based_ds_indices;
}

void GraphLine::updateXIndicesAndGraphPoints(
    const std::vector<size_t>& update_only_these_indices) {
  // x_lim must be set to calculate the xdata.
  jassert(m_common_plot_params->x_lim);

  // x_data empty.
  jassert(!m_x_data.empty());

  updateXIndicesAndGraphPointsIntern(update_only_these_indices);
}

void GraphLine::updateYIndicesAndGraphPoints(
    const std::vector<size_t>& update_only_these_indices) {
  // x_lim must be set to calculate the xdata.
  jassert(m_common_plot_params->y_lim);

  // y_data empty.
  jassert(!m_y_data.empty());

  updateYIndicesAndGraphPointsIntern(update_only_these_indices);
}

void GraphLine::updateXIndicesAndGraphPointsIntern(
    const std::vector<size_t>& update_only_these_indices) {
  const std::lock_guard<std::recursive_mutex> lock(plot_mutex);

  switch (m_common_plot_params->downsampling_type) {
    case DownsamplingType::no_downsampling:
      m_x_based_ds_indices.resize(m_x_data.size());

      std::iota(m_x_based_ds_indices.begin(), m_x_based_ds_indices.end(), 0u);
      break;

    case DownsamplingType::x_downsampling:
      Downsampler<float>::calculateXBasedDSIdxs(*m_common_plot_params, m_x_data,
                                                m_x_based_ds_indices);

      break;

    case DownsamplingType::xy_downsampling:
      Downsampler<float>::calculateXBasedDSIdxs(*m_common_plot_params, m_x_data,
                                                m_x_based_ds_indices);
      return;
      break;

    default:
      break;
  }

  auto lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);
  lnf->updateXGraphPoints(update_only_these_indices, *m_common_plot_params,
                          m_x_data, m_x_based_ds_indices, m_graph_points);
}

void GraphLine::updateYIndicesAndGraphPointsIntern(
    const std::vector<size_t>& update_only_these_indices) {
  auto lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);
  const std::lock_guard<std::recursive_mutex> lock(plot_mutex);

  m_xy_based_ds_indices = m_x_based_ds_indices;

  switch (m_common_plot_params->downsampling_type) {
    case DownsamplingType::no_downsampling:
      break;

    case DownsamplingType::x_downsampling:
      break;

    case DownsamplingType::xy_downsampling:
      Downsampler<float>::calculateXYBasedDSIdxs(*m_common_plot_params,
                                                 m_x_based_ds_indices, m_y_data,
                                                 m_xy_based_ds_indices);

      lnf->updateXGraphPoints(update_only_these_indices, *m_common_plot_params,
                              m_x_data, m_xy_based_ds_indices, m_graph_points);
      break;

    default:
      break;
  }

  lnf->updateYGraphPoints(update_only_these_indices, *m_common_plot_params,
                          m_y_data, m_xy_based_ds_indices, m_graph_points);
}

void GraphLine::updateXYGraphPoints() {
  auto lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);
  lnf->updateXGraphPoints({}, *m_common_plot_params, m_x_data,
                          m_xy_based_ds_indices, m_graph_points);
  lnf->updateYGraphPoints({}, *m_common_plot_params, m_y_data,
                          m_xy_based_ds_indices, m_graph_points);
}

void GraphSpread::resized() {}

void GraphSpread::paint(juce::Graphics& g) {
  if (m_lookandfeel) {
    const std::lock_guard<std::recursive_mutex> lock(plot_mutex);

    auto lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);
    lnf->drawSpread(g, m_lower_bound, m_upper_bound, m_spread_colour);
  }
}

void GraphSpread::lookAndFeelChanged() {
  if (auto* lnf = dynamic_cast<Plot::LookAndFeelMethods*>(&getLookAndFeel())) {
    m_lookandfeel = lnf;
  } else {
    m_lookandfeel = nullptr;
  }
}

}  // namespace cmp
