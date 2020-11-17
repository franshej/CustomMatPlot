#include "spl_plot.h"
#include <stdexcept>

#define sc_int(val) static_cast<int>(val)

Plot::Plot(){};

static std::pair<float, float>
findMinMaxValues(const std::vector<std::vector<float>> &xy_data) {
  auto max_value = std::numeric_limits<float>::min();
  auto min_value = std::numeric_limits<float>::max();

  for (const auto &single_data : xy_data) {
    const auto &current_max =
        *std::max_element(single_data.begin(), single_data.end());
    max_value = current_max > max_value ? current_max : max_value;
    const auto &current_min =
        *std::min_element(single_data.begin(), single_data.end());
    min_value = current_min < min_value ? current_min : min_value;
  }
  return {min_value, max_value};
}

static void
setXLimGraphLines(const std::vector<std::unique_ptr<GraphLine>> &graph_lines,
                  const float &min, const float &max) {
  for (const auto &graph_line : graph_lines) {
    graph_line->xLim(min, max);
  }
}

static void
setYLimGraphLines(const std::vector<std::unique_ptr<GraphLine>> &graph_lines,
                  const float &min, const float &max) {
  for (const auto &graph_line : graph_lines) {
    graph_line->yLim(min, max);
  }
}

void Plot::setAutoXScale(const std::vector<std::vector<float>> &x_data) {
  const auto &[min, max] = findMinMaxValues(x_data);
  setXLimGraphLines(m_graph_lines, min, max);
}

void Plot::setAutoYScale(const std::vector<std::vector<float>> &y_data) {
  const auto &[min, max] = findMinMaxValues(y_data);
  setYLimGraphLines(m_graph_lines, min, max);
}

void Plot::xLim(const float &min, const float &max) {
  setXLimGraphLines(m_graph_lines, min, max);
  m_x_autoscale = false;
}

void Plot::yLim(const float &min, const float &max) {
  setYLimGraphLines(m_graph_lines, min, max);
  m_y_autoscale = false;
}

void Plot::resized() {
  const auto &width = static_cast<float>(getWidth());
  const auto &height = static_cast<float>(getHeight());

  m_plot_area = juce::Rectangle<int>(0, 0, sc_int(width), sc_int(height));
  m_graph_area = juce::Rectangle<int>(
      sc_int(0.1f * width), sc_int(0.05f * height),
      sc_int(width - (0.2f * width)), sc_int(height - (0.1f * height)));

  for (const auto &graph_line : m_graph_lines) {
    graph_line->setBounds(m_graph_area);
  }

  if (m_grid != nullptr) {
    m_grid->setBounds(m_plot_area);
	m_grid->setGraphBounds(m_graph_area);
  }

  updateYDataGraph();
  updateXDataGraph();
}

LinearPlot::LinearPlot(const int x, const int y, const int width,
                       const int height) {
  setBounds(x, y, width, height);
  m_grid = std::make_unique<Grid>();
  addAndMakeVisible(m_grid.get());
}

LinearPlot::LinearPlot() {
  m_grid = std::make_unique<Grid>();
  addAndMakeVisible(m_grid.get());
}

void Plot::paint(juce::Graphics &g) {}

void LinearPlot::updateYData(const std::vector<std::vector<float>> &y_data) {
  if (!y_data.empty()) {
    if (y_data.size() != m_graph_lines.size()) {
      m_graph_lines.resize(y_data.size());

      for (auto &graph_line : m_graph_lines) {
        graph_line = std::make_unique<LinearGraphLine>();
        addAndMakeVisible(graph_line.get());
        graph_line->setBounds(m_graph_area);
      }
    }

    if (y_data.size() != m_y_data.size()) {
      m_y_data.resize(y_data.size());
    }

    std::copy(y_data.begin(), y_data.end(), m_y_data.begin());
    updateYDataGraph();
  }
}

void LinearPlot::updateXData(const std::vector<std::vector<float>> &x_data) {
  if (!x_data.empty()) {
    if (x_data.size() != m_graph_lines.size()) {
      throw std::invalid_argument(
          "Numbers of x_data vectors must be the same amount as y_data "
          "vectors. Make sure to set y_values first.");
    }

    if (x_data.size() != m_x_data.size()) {
      m_x_data.resize(x_data.size());
    }

    std::copy(x_data.begin(), x_data.end(), m_x_data.begin());
    updateXDataGraph();
  }
}

void Plot::updateYDataGraph() {
  if (!m_y_data.empty()) {
    if (m_y_autoscale) {
      setAutoYScale(m_y_data);
    }

    if (m_x_autoscale && m_x_data.empty()) {
      setAutoXScale({{0, float(m_y_data[0].size())}});
    }

    auto i = 0u;
    for (const auto &graph_line : m_graph_lines) {
      graph_line->updateYValues(&m_y_data[i]);
      i++;
    }
  }
}
void Plot::updateXDataGraph() {
  if (!m_x_data.empty()) {
    if (m_x_autoscale) {
      setAutoXScale(m_x_data);
    }

    auto i = 0u;
    for (const auto &graph_line : m_graph_lines) {
      graph_line->updateXValues(&m_x_data[i]);
      i++;
    }
  }
}
