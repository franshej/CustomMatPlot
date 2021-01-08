#include "spl_graph_line.h"
#include <stdexcept>

GraphLine::GraphLine()
    : m_graph_colour(juce::Colour(std::rand() % 100 + 100,
                                  std::rand() % 100 + 100,
                                  std::rand() % 100 + 100)) {}

void GraphLine::xLim(const float &min, const float &max) {
  if (min > max)
    throw std::invalid_argument("xLim min value must be lower than max value.");

  if (abs(max - min) > std::numeric_limits<float>::epsilon()) {
    m_x_min = min;
    m_x_max = max;
  } else {
    throw std::invalid_argument(
        "The min and max value of x_values must not be the same.");
  }
}

void GraphLine::yLim(const float &min, const float &max) {
  if (min > max)
    throw std::invalid_argument("yLim min value must be lower than max value.");

  if (abs(max - min) < std::numeric_limits<float>::epsilon()) {
    m_y_min = 0;
    m_y_max = std::numeric_limits<float>::max();
  } else {
    m_y_min = min;
    m_y_max = max;
  }
}

void GraphLine::resized(){};

struct PathList {
  std::vector<juce::Path> paths;
  juce::Colour colour;
};

void GraphLine::paint(juce::Graphics &g) {
  calculateYData();
  calculateXData();
  juce::Path graph_path;

  if (m_graph_points.size() > 1) {
    graph_path.startNewSubPath(m_graph_points[0]);
    std::for_each(
        m_graph_points.begin() + 1, m_graph_points.end(),
        [&](const juce::Point<float> &point) { graph_path.lineTo(point); });

    g.setColour(m_graph_colour);

    g.strokePath(
        graph_path,
        juce::PathStrokeType(1.0f, juce::PathStrokeType::JointStyle::mitered,
                             juce::PathStrokeType::EndCapStyle::rounded));
  }
}
void GraphLine::updateYValues(const std::vector<float> *y_data) {
  if (!nextYBlockReady) {
    m_y_data = y_data;
    nextYBlockReady = true;
  }
}

void GraphLine::updateXValues(const std::vector<float> *x_values) {
  if (!nextXBlockReady) {
    m_x_data = x_values;
    nextXBlockReady = true;
  }
}

void LinearGraphLine::calculateXData() {

  if (nextXBlockReady && !m_x_data->empty()) {
    if (m_graph_points.size() != m_x_data->size()) {
      throw std::invalid_argument(
          "x_values must be the same length as y_values.");
    }

    const auto &x_scale = static_cast<float>(getWidth()) / (m_x_max - m_x_min);
    const auto &offset_x =
        static_cast<float>(/* getX()) */ -(m_x_min * x_scale));

    auto i = 0u;
    for (const auto &x : *m_x_data) {
      m_graph_points[i].setX(offset_x + (x * x_scale));
      i++;
    }

    nextXBlockReady = false;
  }
}

void LinearGraphLine::calculateYData() {
  if (nextYBlockReady) {

    if (m_graph_points.size() != m_y_data->size()) {
      m_graph_points.resize(m_y_data->size());

      if (!m_x_data) {
        const auto &x_scale =
            static_cast<float>(getWidth()) / (m_x_max - m_x_min);
        const auto &offset_x =
            static_cast<float>(/* getX()) */ -(m_x_min * x_scale));

        auto i = 0u;
        for (auto &point : m_graph_points) {
          point.setX(offset_x + (i++ * x_scale));
        }
      }
    }

    const auto &y_scale = static_cast<float>(getHeight()) / (m_y_max - m_y_min);
    const auto &y_offset = m_y_min;

    const auto offset_y =
        static_cast<float>(getHeight() /* + getY()) */ + (y_offset * y_scale));

    auto i = 0u;
    for (const auto &y : *m_y_data) {
      m_graph_points[i].setY(offset_y - (y * y_scale));
      i++;
    }

    nextYBlockReady = false;
  }
}

void LogXGraphLine::calculateYData() {
  if (nextYBlockReady) {

    if (m_x_min <= 0) {
      throw std::invalid_argument("Minimum x value must be > 0.");
    }

    if (m_graph_points.size() != m_y_data->size()) {
      m_graph_points.resize(m_y_data->size());

      if (!m_x_data) {
        const auto &x_scale =
            static_cast<float>(getWidth()) / (m_x_max - m_x_min);
        const auto &offset_x = static_cast<float>(
            /* getX()) */ -(m_x_min * x_scale));

        auto i = 1u;
        for (auto &point : m_graph_points) {
          point.setX(offset_x + (i++ * x_scale));
        }
      }
    }

    const auto width = static_cast<float>(getWidth());
    const auto y_ratio = m_x_max / static_cast<float>(m_y_data->size());

    auto xToYData = [&](const float &x_pos) {
      const auto y = m_x_min * pow((m_x_max / m_x_min), ((x_pos) / (width)));
      const auto y_index = static_cast<int>(std::floor(y / y_ratio));

      return (*m_y_data)[y_index >= m_y_data->size() ? m_y_data->size() - 1
                                                     : y_index];
    };

    const auto &y_scale = static_cast<float>(getHeight()) / (m_y_max - m_y_min);
    const auto &y_offset = m_y_min;

    const auto offset_y =
        static_cast<float>(getHeight() + (y_offset * y_scale));

    const auto &x_scale = static_cast<float>(getWidth()) / (m_x_max - m_x_min);
    const auto &offset_x = static_cast<float>(-(m_x_min * x_scale));

    auto i = 0u;
    for (const auto &x : *m_x_data) {
      const auto x_scaled = offset_x + (x * x_scale);
      m_graph_points[i].setY(offset_y - (xToYData(x_scaled) * y_scale));
      i++;
    }

    nextYBlockReady = false;
  }
}

void LogXGraphLine::calculateXData() {
  if (nextXBlockReady && !m_x_data->empty()) {
    if (m_graph_points.size() != m_x_data->size()) {
      throw std::invalid_argument(
          "x_values must be the same length as y_values.");
    }

    const auto &x_scale = static_cast<float>(getWidth()) / (m_x_max - m_x_min);
    const auto &offset_x = static_cast<float>(-(m_x_min * x_scale));

    auto i = 0u;
    for (const auto &x : *m_x_data) {
      m_graph_points[i].setX(offset_x + (x * x_scale));
      i++;
    }

    nextXBlockReady = false;
  }
}