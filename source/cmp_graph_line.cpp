/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "cmp_graph_line.h"

#include <mutex>
#include <numeric>
#include <stdexcept>

#include "cmp_downsampler.h"
#include "cmp_plot.h"

namespace cmp {

void GraphLine::setColour(const juce::Colour graph_colour) {
  m_graph_attributes.graph_colour = graph_colour;
}

std::pair<juce::Point<float>, juce::Point<float>>
GraphLine::findClosestGraphPointTo(const juce::Point<float>& this_graph_point,
                                   bool check_only_distance_from_x) const {
  // No graph points.
  jassert(!m_graph_points.empty());

  auto closest_graph_point = juce::Point<float>();
  auto closest_data_point = juce::Point<float>();

  auto closest_distance = std::numeric_limits<float>::max();
  std::size_t i = 0u;
  for (const auto& graph_point : m_graph_points) {
    const auto current_distance =
        check_only_distance_from_x
            ? abs(graph_point.getX() - this_graph_point.getX())
            : graph_point.getDistanceSquaredFrom(this_graph_point);
    if (current_distance < closest_distance) {
      closest_distance = current_distance;
      closest_graph_point = graph_point;
      closest_data_point = juce::Point<float>(
          m_x_data[m_x_based_ds_indices[i]], m_y_data[m_x_based_ds_indices[i]]);
    }
    i++;
  }
  return {closest_graph_point, closest_data_point};
}

juce::Point<float> GraphLine::findClosestDataPointTo(
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
    const auto current_x_dist = abs(x - this_data_point.getX());

    if (current_x_dist < nearest_x_dist) {
      nearest_x_dist = current_x_dist;
      nearest_i = i;
    }
  }

  const auto closest_data_point =
      juce::Point<float>(m_x_data[nearest_i], m_y_data[nearest_i]);

  return closest_data_point;
}

juce::Colour GraphLine::getColour() const noexcept {
  return m_graph_attributes.graph_colour.value();
}

void GraphLine::resized(){};

void GraphLine::paint(juce::Graphics& g) {
  const GraphLineDataView graph_line_data(m_y_data, m_x_data, m_graph_points,
                                          m_x_based_ds_indices,
                                          m_graph_attributes);

  if (m_lookandfeel) {
    const std::lock_guard<std::recursive_mutex> lock(plot_mutex);

    auto lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);
    lnf->drawGraphLine(g, graph_line_data);
  }
}

void GraphLine::lookAndFeelChanged() {
  if (auto* lnf = dynamic_cast<Plot::LookAndFeelMethods*>(&getLookAndFeel())) {
    m_lookandfeel = lnf;
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

const std::vector<float>& GraphLine::getYValues() const noexcept {
  return m_y_data;
}

const std::vector<float>& GraphLine::getXValues() const noexcept {
  return m_x_data;
}

const GraphPoints& GraphLine::getGraphPoints() const noexcept {
  return m_graph_points;
}

void GraphLine::updateXGraphPoints(
    const CommonPlotParameterView common_plot_params) {
  // x_lim must be set to calculate the xdata.
  jassert(common_plot_params.x_lim);

  // x_data empty.
  jassert(!m_x_data.empty());

  updateXGraphPointsIntern(common_plot_params);
}

void GraphLine::updateYGraphPoints(
    const CommonPlotParameterView common_plot_params) {
  // x_lim must be set to calculate the xdata.
  jassert(common_plot_params.y_lim);

  // y_data empty.
  jassert(!m_y_data.empty());

  updateYGraphPointsIntern(common_plot_params);
}

void GraphLine::updateXGraphPointsIntern(
    const CommonPlotParameterView common_plot_params) noexcept {
  if (m_lookandfeel) {
    const std::lock_guard<std::recursive_mutex> lock(plot_mutex);

    switch (common_plot_params.downsampling_type) {
      case DownsamplingType::no_downsampling:
        m_x_based_ds_indices.resize(m_x_data.size());

        std::iota(m_x_based_ds_indices.begin(), m_x_based_ds_indices.end(), 0u);
        break;

      case DownsamplingType::x_downsampling:
        Downsampler<float>::calculateXBasedDSIdxs(common_plot_params, m_x_data,
                                                  m_x_based_ds_indices);

        break;

      case DownsamplingType::xy_downsampling:
        Downsampler<float>::calculateXBasedDSIdxs(common_plot_params, m_x_data,
                                                  m_x_based_ds_indices);
        return;
        break;

      default:
        break;
    }

    auto lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);
    lnf->updateXGraphPoints(common_plot_params.graph_bounds,
                            common_plot_params.x_lim, m_x_data,
                            m_x_based_ds_indices, m_graph_points);
  }
}

void GraphLine::updateYGraphPointsIntern(
    const CommonPlotParameterView common_plot_params) noexcept {
  if (m_lookandfeel) {
    auto lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);

    const std::lock_guard<std::recursive_mutex> lock(plot_mutex);

    auto& graph_point_indices = m_x_based_ds_indices;

    switch (common_plot_params.downsampling_type) {
      case DownsamplingType::no_downsampling:
        break;

      case DownsamplingType::x_downsampling:
        break;

      case DownsamplingType::xy_downsampling:
        Downsampler<float>::calculateXYBasedDSIdxs(
            common_plot_params, m_x_based_ds_indices, m_y_data,
            m_xy_based_ds_indices);

        graph_point_indices = m_xy_based_ds_indices;

        lnf->updateXGraphPoints(common_plot_params.graph_bounds,
                                common_plot_params.x_lim, m_x_data,
                                graph_point_indices, m_graph_points);
        break;

      default:
        break;
    }

    lnf->updateYGraphPoints(common_plot_params.graph_bounds,
                            common_plot_params.y_lim, m_y_data,
                            graph_point_indices, m_graph_points);
  }
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
