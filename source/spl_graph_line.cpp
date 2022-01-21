#include "spl_graph_line.h"

#include <stdexcept>

#include "spl_plot.h"

namespace scp {

void GraphLine::setColour(const juce::Colour graph_colour) {
  m_graph_colour = graph_colour;
}

std::pair<juce::Point<float>, juce::Point<float>>
GraphLine::findClosestGraphPointTo(
    const juce::Point<float>& this_graph_point,
    bool check_only_distance_from_x) const noexcept {
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
      closest_data_point =
          juce::Point<float>(m_x_data[m_graph_point_indices[i]],
                             m_y_data[m_graph_point_indices[i]]);
    }
    i++;
  }
  return {closest_graph_point, closest_data_point};
}

juce::Point<float> GraphLine::findClosestDataPointTo(
    const juce::Point<float>& this_data_point,
    bool check_only_distance_from_x) const noexcept {
  // No y_data empty.
  jassert(!m_x_data.empty());

  // Uninitialized, execute 'updateXGraphPoints' atleast once.
  jassert(m_state == State::Initialized);

  auto nearest_x_dist = std::numeric_limits<float>::max();
  std::size_t nearest_i = 0u;

  for (const auto i : m_graph_point_indices) {
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

juce::Colour GraphLine::getColour() const noexcept { return m_graph_colour; }

void GraphLine::resized() { m_state = State::Uninitialized; };

void GraphLine::paint(juce::Graphics& g) {
  if (m_lookandfeel) {
    auto lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);
    lnf->drawGraphLine(g, m_graph_points, m_dashed_lengths, m_graph_colour);
  }
}

void GraphLine::lookAndFeelChanged() {
  if (auto* lnf = dynamic_cast<Plot::LookAndFeelMethods*>(&getLookAndFeel())) {
    m_lookandfeel = lnf;
  } else {
    m_lookandfeel = nullptr;
  }
}

void GraphLine::setYValues(const std::vector<float>& y_data) noexcept {
  if (m_y_data.size() != y_data.size()) m_y_data.resize(y_data.size());
  std::copy(y_data.begin(), y_data.end(), m_y_data.begin());

  m_graph_point_indices.resize(y_data.size());
}

void GraphLine::setXValues(const std::vector<float>& x_data) noexcept {
  if (m_x_data.size() != x_data.size()) m_x_data.resize(x_data.size());
  std::copy(x_data.begin(), x_data.end(), m_x_data.begin());

  m_graph_point_indices.resize(x_data.size());
}

void GraphLine::setDashedPath(
    const std::vector<float>& dashed_lengths) noexcept {
  m_dashed_lengths = dashed_lengths;
}

const std::vector<float>& GraphLine::getYValues() noexcept { return m_y_data; }

const std::vector<float>& GraphLine::getXValues() noexcept { return m_x_data; }

const GraphPoints& GraphLine::getGraphPoints() noexcept {
  return m_graph_points;
}

void GraphLine::updateXGraphPoints(const CommonPlotParameterView& common_plot_params) {
  // x_lim must be set to calculate the xdata.
  jassert(common_plot_params.x_lim);

  // x_data empty.
  jassert(!m_x_data.empty());

  if (m_x_data.size() != m_graph_points.size()) {
    m_graph_points.resize(m_x_data.size());
  }

  updateXGraphPointsIntern(common_plot_params);

  m_state = State::Initialized;
}

void GraphLine::updateYGraphPoints(const CommonPlotParameterView& common_plot_params) {
  // x_lim must be set to calculate the xdata.
  jassert(common_plot_params.y_lim);

  // y_data empty.
  jassert(!m_y_data.empty());

  // Uninitialized, execute 'updateXGraphPoints' atleast once.
  jassert(m_state == State::Initialized);

  updateYGraphPointsIntern(common_plot_params);
}

void GraphLine::updateXGraphPointsIntern(
    const CommonPlotParameterView& common_plot_params) noexcept {
  if (m_lookandfeel) {
    auto lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);
    lnf->updateXGraphPointsAndIndices(common_plot_params.graph_bounds,
                                      common_plot_params.x_lim, m_x_data,
                                      m_graph_point_indices, m_graph_points);
  }
}

void GraphLine::updateYGraphPointsIntern(
    const CommonPlotParameterView& common_plot_params) noexcept {
  if (m_lookandfeel) {
    auto lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);
    lnf->updateYGraphPoints(common_plot_params.graph_bounds, common_plot_params.y_lim,
                            m_y_data, m_graph_point_indices, m_graph_points);
  }
}

}  // namespace scp
