#include "spl_graph_line.h"

#include <stdexcept>

#include "spl_plot.h"

namespace scp {
GraphLine::GraphLine() : m_graph_type(Plot::GraphType::GraphLine), m_id(0u) {}
GraphLine::GraphLine(const GraphType graph_type)
    : m_graph_type(graph_type), m_id(0u) {}

void GraphLine::setXLim(const float min, const float max) {
  Lim_f x_lim;

  if (min > max)
    throw std::invalid_argument("Min value must be lower than max value.");

  if (abs(max - min) > std::numeric_limits<float>::epsilon()) {
    x_lim.min = min;
    x_lim.max = max;
  } else {
    throw std::invalid_argument(
        "The min and max value of x_values must not be the same.");
  }
  m_x_lim = x_lim;
}

void GraphLine::setYLim(const float min, const float max) {
  Lim_f y_lim;

  if (min > max)
    throw std::invalid_argument(
        "setYLim min value must be lower than max value.");

  if (abs(max - min) < std::numeric_limits<float>::epsilon()) {
    y_lim.min = 0;
    y_lim.max = std::numeric_limits<float>::max();
  } else {
    y_lim.min = min;
    y_lim.max = max;
  }
  m_y_lim = y_lim;
}

void GraphLine::resized(){};

void GraphLine::paint(juce::Graphics& g) {
  if (m_lookandfeel) {
    auto lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);
    const auto graph_type = static_cast<Plot::GraphType>(m_graph_type);
    lnf->drawGraphLine(g, m_graph_points, m_dashed_lengths, graph_type, m_id);
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

  if (m_graph_points.size() != m_y_data.size()) {
    m_graph_points.resize(m_y_data.size());
  }
}

void GraphLine::setXValues(const std::vector<float>& x_data) noexcept {
  if (m_x_data.size() != x_data.size()) m_x_data.resize(x_data.size());
  std::copy(x_data.begin(), x_data.end(), m_x_data.begin());

  if (m_x_data.size() != m_graph_points.size()) {
    m_graph_points.resize(m_x_data.size());
  }
}

void GraphLine::setID(const std::size_t id) noexcept { m_id = id; }

std::size_t GraphLine::getID() const noexcept { return m_id; }

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
    jassert("x_lim must be set to calculate the xdata.");
    return;
  }

  if (m_x_data.empty()) {
    jassert("xdata must be added to calculate");
    return;
  }

  updateXGraphPointsIntern();
}

void GraphLine::updateYGraphPoints() {
    if (!m_y_lim) {
        jassert("y_lim must be set to calculate the ydata.");
        return;
    }

    if (m_y_data.empty()) {
        return;
    }

    updateYGraphPointsIntern();
}

juce::Colour GraphLine::getGraphColourFromIndex(const std::size_t index) {
  return juce::Colour();
}

void LinearGraphLine::updateXGraphPointsIntern() noexcept {
  if (m_lookandfeel) {
    auto lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);
    lnf->updateXGraphPoints(getBounds(), Plot::Scaling::linear, m_x_lim,
                            m_x_data, m_graph_points);
  }
}

void LinearGraphLine::updateYGraphPointsIntern() noexcept {
  if (m_lookandfeel) {
    auto lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);
    lnf->updateYGraphPoints(getBounds(), Plot::Scaling::linear, m_y_lim,
                            m_y_data, m_graph_points);
  }
}

void LogXGraphLine::updateXGraphPointsIntern() noexcept {
  if (m_lookandfeel) {
    auto lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);
    lnf->updateXGraphPoints(getBounds(), Plot::Scaling::logarithmic, m_x_lim,
                            m_x_data, m_graph_points);
  }
}

void LogXGraphLine::updateYGraphPointsIntern() noexcept {
  if (m_lookandfeel) {
    auto lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);
    lnf->updateYGraphPoints(getBounds(), Plot::Scaling::linear, m_y_lim,
                            m_y_data, m_graph_points);
  }
}
}  // namespace scp
