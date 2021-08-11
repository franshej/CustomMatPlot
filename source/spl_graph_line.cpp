#include "spl_graph_line.h"

#include <stdexcept>

void GraphLine::xLim(const float &min, const float &max) {
  if (min > max)
    throw std::invalid_argument("xLim min value must be lower than max value.");

  if (abs(max - min) > std::numeric_limits<float>::epsilon()) {
    m_x_min_lim = min;
    m_x_max_lim = max;
  } else {
    throw std::invalid_argument(
        "The min and max value of x_values must not be the same.");
  }
}

void GraphLine::yLim(const float &min, const float &max) {
  if (min > max)
    throw std::invalid_argument("yLim min value must be lower than max value.");

  if (abs(max - min) < std::numeric_limits<float>::epsilon()) {
    m_y_min_lim = 0;
    m_y_max_lim = std::numeric_limits<float>::max();
  } else {
    m_y_min_lim = min;
    m_y_max_lim = max;
  }
}

void GraphLine::resized(){};

void GraphLine::paint(juce::Graphics &g) {
  juce::Path graph_path;
  juce::PathStrokeType p_type(1.0f, juce::PathStrokeType::JointStyle::mitered,
                              juce::PathStrokeType::EndCapStyle::rounded);

  if (m_graph_points.size() > 1) {
    graph_path.startNewSubPath(m_graph_points[0]);
    std::for_each(
        m_graph_points.begin() + 1, m_graph_points.end(),
        [&](const juce::Point<float> &point) { graph_path.lineTo(point); });

    if (!m_dashed_lengths.empty()) {
      p_type.createDashedStroke(graph_path, graph_path, m_dashed_lengths.data(),
                                m_dashed_lengths.size());
    }

    g.setColour(m_graph_colour);
    g.strokePath(graph_path, p_type);
  }
}
void GraphLine::setYValues(const std::vector<float> &y_data) {
  m_y_data = y_data;

  if (m_graph_points.size() != m_y_data.size()) {
    m_graph_points.resize(m_y_data.size());
  }
}

void GraphLine::setXValues(const std::vector<float> &x_data) {
  m_x_data = x_data;

  if (m_x_data.size() != m_graph_points.size()) {
    m_graph_points.resize(m_x_data.size());
  }
}

void GraphLine::setDashedPath(const std::vector<float> &dashed_lengths) {
  m_dashed_lengths = dashed_lengths;
}

const std::vector<float> &GraphLine::getYValues() { return m_y_data; }

const std::vector<float> &GraphLine::getXValues() { return m_x_data; }

void GraphLine::calculateXData() {
  if (m_x_max_lim - m_x_min_lim <= 0) {
    return;
  }

  if (m_x_data.empty()) {
    return;
  }

  calculateXDataIntern();
}

void GraphLine::calculateYData() {
  if (m_y_max_lim - m_y_min_lim <= 0) {
    return;
  }

  if (m_y_data.empty()) {
    return;
  }

  calculateYDataIntern();
}

void LinearGraphLine::calculateXDataIntern() {
  const auto x_scale =
      static_cast<float>(getWidth()) / (m_x_max_lim - m_x_min_lim);
  const auto offset_x =
      static_cast<float>(/* getX()) */ -(m_x_min_lim * x_scale));

  std::size_t i = 0u;
  for (const auto &x : m_x_data) {
    m_graph_points[i].setX(offset_x + (x * x_scale));
    i++;
  }
}

void LinearGraphLine::calculateYDataIntern() {
  const auto y_scale =
      static_cast<float>(getHeight()) / (m_y_max_lim - m_y_min_lim);
  const auto y_offset = m_y_min_lim;

  const auto offset_y = float(getHeight()) + (y_offset * y_scale);

  std::size_t i = 0u;
  for (const auto &y : m_y_data) {
    m_graph_points[i].setY(offset_y - (y * y_scale));
    i++;
  }
}

void LogXGraphLine::calculateYDataIntern() {
  const auto y_scale =
      static_cast<float>(getHeight()) / (m_y_max_lim - m_y_min_lim);
  const auto y_offset = m_y_min_lim;

  const auto offset_y = float(getHeight()) + (y_offset * y_scale);

  std::size_t i = 0u;
  for (const auto &y : m_y_data) {
    m_graph_points[i].setY(offset_y - y * y_scale);
    i++;
  }
}

void LogXGraphLine::calculateXDataIntern() {
  const auto width = static_cast<float>(getWidth());

  auto xToXPos = [&](const float x) {
    return width * (log(x / m_x_min_lim) / log(m_x_max_lim / m_x_min_lim));
  };

  std::size_t i = 0u;
  for (const auto &x : m_x_data) {
    if (x < m_x_max_lim) {
      m_graph_points[i].setX(xToXPos(x));
      i++;
    }
  }
}
