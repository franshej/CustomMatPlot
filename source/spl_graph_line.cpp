#include "spl_graph_line.h"
#include "spl_plot.h"

#include <stdexcept>

namespace scp {
void GraphLine::setXLim(const float min, const float max) {
  scp::Lim_f x_lim;

  if (min > max)
    throw std::invalid_argument(
        "setXLim min value must be lower than max value.");

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
  scp::Lim_f y_lim;

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
    auto lnf = static_cast<scp::Plot::LookAndFeelMethods*>(m_lookandfeel);
    lnf->drawGraphLine(g, m_graph_points, m_dashed_lengths);
  }
}

void GraphLine::lookAndFeelChanged() {
  if (auto* lnf =
          dynamic_cast<scp::Plot::LookAndFeelMethods*>(&getLookAndFeel())) {
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

void GraphLine::setDashedPath(
    const std::vector<float>& dashed_lengths) noexcept {
  m_dashed_lengths = dashed_lengths;
}

void GraphLine::setGraphColour(const juce::Colour& graph_colour) noexcept {
  m_graph_colour = graph_colour;
}

const std::vector<float>& GraphLine::getYValues() noexcept { return m_y_data; }

const std::vector<float>& GraphLine::getXValues() noexcept { return m_x_data; }

const GraphPoints& GraphLine::getGraphPoints() noexcept {
  return m_graph_points;
}

void GraphLine::calculateXData() {
  if (!m_x_lim) {
    jassert("x_lim must be set to calculate the xdata.");
    return;
  }

  if (m_x_data.empty()) {
    jassert("xdata must be added to calculate");
    return;
  }

  calculateXDataIntern(m_graph_points);
}

juce::Colour GraphLine::getGraphColourFromIndex(const std::size_t index) {
  return juce::Colour();
}

void GraphLine::calculateYData() {
  if (!m_y_lim) {
    jassert("y_lim must be set to calculate the ydata.");
    return;
  }

  if (m_y_data.empty()) {
    return;
  }

  calculateYDataIntern(m_graph_points);
}

void LinearGraphLine::calculateXDataIntern(GraphPoints& graph_points) noexcept {
  const auto x_lim = scp::Lim_f(m_x_lim);

  const auto x_scale = static_cast<float>(getWidth()) / (x_lim.max - x_lim.min);
  const auto offset_x = static_cast<float>(-(x_lim.min * x_scale));

  std::size_t i = 0u;
  for (const auto& x : m_x_data) {
    graph_points[i].setX(offset_x + (x * x_scale));
    i++;
  }
}

void LinearGraphLine::calculateYDataIntern(GraphPoints& graph_points) noexcept {
  const auto y_lim = scp::Lim_f(m_y_lim);

  const auto y_scale =
      static_cast<float>(getHeight()) / (y_lim.max - y_lim.min);
  const auto y_offset = y_lim.min;

  const auto offset_y = float(getHeight()) + (y_offset * y_scale);

  std::size_t i = 0u;
  for (const auto& y : m_y_data) {
    graph_points[i].setY(offset_y - (y * y_scale));
    i++;
  }
}

void LogXGraphLine::calculateXDataIntern(GraphPoints& graph_points) noexcept {
  const auto& xlim = scp::Lim_f(m_x_lim);
  const auto width = static_cast<float>(getWidth());

  auto xToXPos = [&](const float x) {
    return width * (log(x / xlim.min) / log(xlim.max / xlim.min));
  };

  std::size_t i = 0u;
  for (const auto& x : m_x_data) {
    if (x < xlim.max) {
      graph_points[i].setX(xToXPos(x));
      i++;
    }
  }
}

void LogXGraphLine::calculateYDataIntern(GraphPoints& graph_points) noexcept {
  const auto y_lim = scp::Lim_f(m_y_lim);

  const auto y_scale =
      static_cast<float>(getHeight()) / (y_lim.max - y_lim.min);
  const auto y_offset = y_lim.min;

  const auto offset_y = float(getHeight()) + (y_offset * y_scale);

  std::size_t i = 0u;
  for (const auto& y : m_y_data) {
    graph_points[i].setY(offset_y - y * y_scale);
    i++;
  }
}
}  // namespace scp
