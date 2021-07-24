#include "spl_plot.h"

#include <stdexcept>

#define sc_int(val) static_cast<int>(val)

static std::pair<float, float> findMinMaxValues(
    const std::vector<std::vector<float>> &data) {
  auto max_value = -std::numeric_limits<float>::max();
  auto min_value = std::numeric_limits<float>::max();

  for (const auto &single_data_vector : data) {
    const auto &current_max =
        *std::max_element(single_data_vector.begin(), single_data_vector.end());
    max_value = current_max > max_value ? current_max : max_value;
    const auto &current_min =
        *std::min_element(single_data_vector.begin(), single_data_vector.end());
    min_value = current_min < min_value ? current_min : min_value;
  }
  return {min_value, max_value};
}

static std::pair<float, float> findMinMaxValues(
    const std::vector<std::unique_ptr<GraphLine>> &graph_lines,
    const bool isXValue) {
  auto max_value = -std::numeric_limits<float>::max();
  auto min_value = std::numeric_limits<float>::max();

  for (const auto &graph : graph_lines) {
    const auto single_data_vector =
        isXValue ? graph->getXValues() : graph->getYValues();

    const auto &current_max =
        *std::max_element(single_data_vector.begin(), single_data_vector.end());
    max_value = current_max > max_value ? current_max : max_value;
    const auto &current_min =
        *std::min_element(single_data_vector.begin(), single_data_vector.end());
    min_value = current_min < min_value ? current_min : min_value;
  }
  return {min_value, max_value};
}

void Plot::updateYLim(const float &min, const float &max) {
  for (auto &graph_line : m_graph_lines) {
    graph_line->yLim(min, max);
  }
  if (m_grid != nullptr) {
    m_grid->yLim(min, max);
  }
}

void Plot::updateXLim(const float &min, const float &max) {
  for (auto &graph_line : m_graph_lines) {
    graph_line->xLim(min, max);
  }
  if (m_grid != nullptr) {
    m_grid->xLim(min, max);
  }
}

void Plot::setAutoXScale() {
  const auto &[min, max] = findMinMaxValues(m_graph_lines, true);
  updateXLim(min, max);
}

void Plot::setAutoYScale() {
  const auto &[min, max] = findMinMaxValues(m_graph_lines, false);
  updateYLim(min, max);
}

void Plot::xLim(const float &min, const float &max) {
  updateXLim(min, max);
  m_x_autoscale = false;
}

void Plot::yLim(const float &min, const float &max) {
  updateYLim(min, max);
  m_y_autoscale = false;
}

void Plot::setXLabel(const std::string &x_label) {
  m_plot_label.setXLabel(x_label);
}

void Plot::setYLabel(const std::string &y_label) {
  m_plot_label.setYLabel(y_label);
}

void Plot::setTitle(const std::string &title) { m_plot_label.setTitle(title); }

void Plot::makeGraphDashed(const std::vector<float> &dashed_lengths,
                           unsigned graph_index) {
  if (graph_index >= m_graph_lines.size()) {
    throw std::invalid_argument(
        "graph_index out of range, updateYData before 'makeGraphDashed'");
  }
  m_graph_lines[graph_index]->setDashedPath(dashed_lengths);
}

void Plot::gridON(const bool grid_on) { m_grid->gridON(grid_on); }

void Plot::resized() {
  const auto &width = static_cast<float>(getWidth());
  const auto &height = static_cast<float>(getHeight());

  m_plot_area = juce::Rectangle<int>(0, 0, sc_int(width), sc_int(height));
  m_graph_area.setBounds(100, 50, width - 150, height - 125);

  for (const auto &graph_line : m_graph_lines) {
    graph_line->setBounds(m_graph_area);
  }

  if (m_grid != nullptr) {
    m_grid->setBounds(m_plot_area);
    m_grid->setGraphBounds(m_graph_area);
  }

  m_plot_label.setBounds(m_plot_area);
  m_plot_label.setGraphArea(m_graph_area);

  for (const auto &graph_line : m_graph_lines) {
    graph_line->calculateYData();
    graph_line->calculateXData();
  }
}

LinearPlot::LinearPlot(const int x, const int y, const int width,
                       const int height) {
  setBounds(x, y, width, height);
  m_grid = std::make_unique<Grid>();
  addAndMakeVisible(m_grid.get());
  addAndMakeVisible(m_plot_label);
}

LinearPlot::LinearPlot() {
  m_grid = std::make_unique<Grid>();
  addAndMakeVisible(m_grid.get());
  addAndMakeVisible(m_plot_label);
}

void Plot::paint(juce::Graphics &g) {}

SemiPlotX::SemiPlotX() {
  m_grid = std::make_unique<SemiLogXGrid>();
  addAndMakeVisible(m_grid.get());
  addAndMakeVisible(m_plot_label);
}

void Plot::updateYData(const std::vector<std::vector<float>> &y_data) {
  if (!y_data.empty()) {
    if (y_data.size() != m_graph_lines.size()) {
      m_graph_lines.resize(y_data.size());
      for (auto &graph_line : m_graph_lines) {
        if (!graph_line) {
          graph_line = getGraphLine();
          addAndMakeVisible(graph_line.get());
          graph_line->setBounds(m_graph_area);
        }
      }
    }

    auto i = 0u;
    for (const auto &graph_line : m_graph_lines) {
      graph_line->setYValues(y_data[i]);
      i++;
    }

    if (m_y_autoscale) {
      setAutoYScale();
    }

    for (auto &graph_line : m_graph_lines) {
      if (graph_line->getXValues().empty()) {
        auto &x_data = std::vector<float>(graph_line->getYValues().size());
        std::iota(x_data.begin(), x_data.end(), 1);
        graph_line->setXValues(x_data);
      }

      if (m_x_autoscale && !m_graph_lines.back()->getXValues().empty()) {
        setAutoXScale();
      }
    }
  }
}

void Plot::updateXData(const std::vector<std::vector<float>> &x_data) {
  if (!x_data.empty()) {
    if (x_data.size() != m_graph_lines.size()) {
      throw std::invalid_argument(
          "Numbers of x_data vectors must be the same amount as y_data "
          "vectors. Make sure to set y_values first.");
    }

    auto i = 0;
    for (const auto &graph : m_graph_lines) {
      const auto y_graph_length = graph->getYValues().size();
      const auto x_graph_length = x_data[i].size();

      if (y_graph_length != x_graph_length) {
        throw std::invalid_argument(
            "Invalid vector length.\nLength of y vector at index " +
            std::to_string(i) + " is " + std::to_string(y_graph_length) +
            "\nLength of x vector at index " + std::to_string(i) + " is " +
            std::to_string(y_graph_length));
      }

      graph->setXValues(x_data[i]);
      i++;
    }
    if (m_x_autoscale) {
      setAutoXScale();
    }
  }
}
