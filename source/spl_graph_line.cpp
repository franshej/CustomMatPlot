#include "spl_graph_line.h"

#include <stdexcept>

#include "spl_plot.h"

namespace scp {

void GraphLine::setColour(const juce::Colour graph_colour) {
  m_graph_colour = graph_colour;
}

std::pair<juce::Point<float>, juce::Point<float>>
GraphLine::findClosestGraphPointTo(
    const juce::Point<float>& point) const noexcept {
  if (m_graph_points.empty()) {
    // No graph points.
    jassertfalse;
  }

  auto closest_point = juce::Point<float>();
  auto closest_data_point = juce::Point<float>();

  auto closest_distance = std::numeric_limits<float>::max();
  std::size_t i = 0u;
  for (const auto& graph_point : m_graph_points) {
    const auto current_distance = graph_point.getDistanceSquaredFrom(point);
    if (current_distance < closest_distance) {
      closest_distance = current_distance;
      closest_point = graph_point;
      closest_data_point =
          juce::Point<float>(m_x_data[m_graph_point_indices[i]],
                             m_y_data[m_graph_point_indices[i]]);
    }
    i++;
  }
  return {closest_point, closest_data_point};
}

juce::Colour GraphLine::getColour() const noexcept { return m_graph_colour; }

void GraphLine::setXLim(const Lim_f& new_x_lim) {
  if (new_x_lim.min > new_x_lim.max)
    throw std::invalid_argument("Min value must be lower than max value.");

  if ((abs(new_x_lim.max - new_x_lim.min) <
       std::numeric_limits<float>::epsilon()))
    UNLIKELY {
      m_x_lim.min = new_x_lim.min - 1;
      m_x_lim.max = new_x_lim.max + 1;
    }
  else {
    m_x_lim = new_x_lim;
  }
}

void GraphLine::setYLim(const Lim_f& new_y_lim) {
  if (new_y_lim.min > new_y_lim.max)
    throw std::invalid_argument("Min value must be lower than max value.");

  if (abs(new_y_lim.max - new_y_lim.min) <
      std::numeric_limits<float>::epsilon())
    UNLIKELY {
      m_y_lim.min = new_y_lim.min - 1;
      m_y_lim.max = new_y_lim.max + 1;
    }
  else {
    m_y_lim = new_y_lim;
  }
}

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

void GraphLine::updateXGraphPoints() {
  if (!m_x_lim) {
    // x_lim must be set to calculate the xdata.
    jassertfalse;
    return;
  }

  if (m_x_data.empty()) {
    // x_data empty.
    jassertfalse;
    return;
  }

  if (m_x_data.size() != m_graph_points.size()) {
    m_graph_points.resize(m_x_data.size());
  }

  updateXGraphPointsIntern();

  m_state = State::Initialized;
}

void GraphLine::updateYGraphPoints() {
  if (!m_y_lim) {
    // y_lim must be set to calculate the ydata.
    jassertfalse;
  }

  if (m_y_data.empty()) {
    // y_data empty.
    jassertfalse;
    return;
  }

  if (m_state == State::Uninitialized) {
    // Uninitialized, execute 'updateXGraphPoints' atleast once.
    jassertfalse;
    return;
  }

  updateYGraphPointsIntern();
}

void GraphLine::updateXGraphPointsIntern() noexcept {
  if (m_lookandfeel) {
    auto lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);
    lnf->updateXGraphPointsAndIndices(getBounds(), m_x_lim, m_x_data,
                                      m_graph_point_indices, m_graph_points);
  }
}

void GraphLine::updateYGraphPointsIntern() noexcept {
  if (m_lookandfeel) {
    auto lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);
    lnf->updateYGraphPoints(getBounds(), m_y_lim, m_y_data,
                            m_graph_point_indices, m_graph_points);
  }
}

}  // namespace scp
