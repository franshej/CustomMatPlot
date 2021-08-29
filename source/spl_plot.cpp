#include <stdexcept>

#include "scp_lookandfeel.h"
#include "spl_label.h"
#include "spl_grid.h"
#include "spl_graph_line.h"

namespace scp {

static std::pair<float, float> findMinMaxValues(
    const std::vector<std::vector<float>>& data) noexcept {
  auto max_value = -std::numeric_limits<float>::max();
  auto min_value = std::numeric_limits<float>::max();

  for (const auto& single_data_vector : data) {
    const auto& current_max =
        *std::max_element(single_data_vector.begin(), single_data_vector.end());
    max_value = current_max > max_value ? current_max : max_value;
    const auto& current_min =
        *std::min_element(single_data_vector.begin(), single_data_vector.end());
    min_value = current_min < min_value ? current_min : min_value;
  }
  return {min_value, max_value};
}

static std::pair<float, float> findMinMaxValues(
    const std::vector<std::unique_ptr<scp::GraphLine>>& graph_lines,
    const bool isXValue) noexcept {
  auto max_value = -std::numeric_limits<float>::max();
  auto min_value = std::numeric_limits<float>::max();

  for (const auto& graph : graph_lines) {
    const auto single_data_vector =
        isXValue ? graph->getXValues() : graph->getYValues();

    const auto& current_max =
        *std::max_element(single_data_vector.begin(), single_data_vector.end());
    max_value = current_max > max_value ? current_max : max_value;
    const auto& current_min =
        *std::min_element(single_data_vector.begin(), single_data_vector.end());
    min_value = current_min < min_value ? current_min : min_value;
  }
  return {min_value, max_value};
}

Plot::~Plot() {
  m_grid->setLookAndFeel(nullptr);
  m_plot_label->setLookAndFeel(nullptr);
  for (auto& graph_line : m_graph_lines) {
    graph_line->setLookAndFeel(nullptr);
  }
}

void Plot::updateYLim(const float min, const float max) {
  for (auto& graph_line : m_graph_lines) {
    graph_line->setYLim(min, max);
  }
  if (m_grid != nullptr) {
    m_grid->setYLim(min, max);
  }
}

void Plot::updateXLim(const float min, const float max) {
  for (auto& graph_line : m_graph_lines) {
    graph_line->setXLim(min, max);
  }
  if (m_grid != nullptr) {
    m_grid->setXLim(min, max);
  }
}

void Plot::initialize() {
  m_grid = getGrid();
  m_plot_label = std::make_unique<PlotLabel>();

  lookAndFeelChanged();
  addAndMakeVisible(m_grid.get());
  addAndMakeVisible(m_plot_label.get());
}

void Plot::setAutoXScale() {
  const auto [min, max] = findMinMaxValues(m_graph_lines, true);
  updateXLim(min, max);
}

void Plot::setAutoYScale() {
  const auto [min, max] = findMinMaxValues(m_graph_lines, false);
  updateYLim(min, max);
}

void Plot::xLim(const float min, const float max) {
  updateXLim(min, max);
  m_x_autoscale = false;
}

void Plot::yLim(const float min, const float max) {
  updateYLim(min, max);
  m_y_autoscale = false;
}

void Plot::setXLabel(const std::string& x_label) {
  m_plot_label->setXLabel(x_label);
}

void Plot::setXLabels(const std::vector<std::string>& x_labels) {
  m_grid->setXLabels(x_labels);
}

void Plot::setYLabels(const std::vector<std::string>& y_labels) {
  m_grid->setYLabels(y_labels);
}

void Plot::setXTicks(const std::vector<float>& x_ticks) {
  m_grid->setXTicks(x_ticks);
}

void Plot::setYTicks(const std::vector<float>& y_ticks) {
  m_grid->setYTicks(y_ticks);
}

void Plot::setYLabel(const std::string& y_label) {
  m_plot_label->setYLabel(y_label);
}

void Plot::setTitle(const std::string& title) { m_plot_label->setTitle(title); }

void Plot::makeGraphDashed(const std::vector<float>& dashed_lengths,
                           unsigned graph_index) {
  if (graph_index >= m_graph_lines.size()) {
    throw std::invalid_argument(
        "graph_index out of range, updateYData before 'makeGraphDashed'");
  }
  m_graph_lines[graph_index]->setDashedPath(dashed_lengths);
}

void Plot::gridON(const bool grid_on, const bool tiny_grid_on) {
  m_grid->setGridON(grid_on, tiny_grid_on);
}

void Plot::resized() {
  if (auto lnf = dynamic_cast<LookAndFeelMethods*>(m_lookandfeel)) {
    const auto plot_area = lnf->getPlotBounds(getBounds());
    const auto graph_area = lnf->getGraphBounds(getBounds());

    for (const auto& graph_line : m_graph_lines) {
      graph_line->setBounds(graph_area);
    }

    if (m_grid != nullptr) {
      m_grid->setBounds(plot_area);
      m_grid->setGridBounds(graph_area);
    }

    m_plot_label->setBounds(plot_area);
    m_plot_label->setGraphArea(graph_area);

    for (const auto& graph_line : m_graph_lines) {
      graph_line->updateYGraphPoints();
      graph_line->updateXGraphPoints();
    }
  }
}

void Plot::paint(juce::Graphics& g) {}

void Plot::parentHierarchyChanged() { lookAndFeelChanged(); }

void Plot::lookAndFeelChanged() {
  if (dynamic_cast<LookAndFeelMethods*>(&getLookAndFeel())) {
    m_lookandfeel = &getLookAndFeel();
    m_lookandfeel_default.reset();
  } else {
    if (!m_lookandfeel_default)
      m_lookandfeel_default = std::make_unique<scp::PlotLookAndFeel>();
    m_lookandfeel = m_lookandfeel_default.get();
  }

  m_grid->setLookAndFeel(m_lookandfeel);
  m_plot_label->setLookAndFeel(m_lookandfeel);
  for (auto& graph_line : m_graph_lines) {
    graph_line->setLookAndFeel(m_lookandfeel);
  }
}

void Plot::updateYData(const std::vector<std::vector<float>>& y_data) {
  if (!y_data.empty()) {
    if (y_data.size() != m_graph_lines.size()) {
      m_graph_lines.resize(y_data.size());
      std::size_t i = 0u;
      for (auto& graph_line : m_graph_lines) {
        if (!graph_line) {
          if (auto lnf = dynamic_cast<LookAndFeelMethods*>(m_lookandfeel)) {
            graph_line = getGraphLine();
            graph_line->setID(i);
            graph_line->setLookAndFeel(m_lookandfeel);
            const auto graph_area = lnf->getGraphBounds(getBounds());
            graph_line->setBounds(graph_area);
            addAndMakeVisible(graph_line.get());
            i++;
          }
        }
      }
    }

    std::size_t i = 0u;
    for (const auto& graph_line : m_graph_lines) {
      graph_line->setYValues(y_data[i]);
      i++;
    }

    if (m_y_autoscale) {
      setAutoYScale();
    }

    for (auto& graph_line : m_graph_lines) {
      if (graph_line->getXValues().empty()) {
        auto& x_data = std::vector<float>(graph_line->getYValues().size());
        std::iota(x_data.begin(), x_data.end(), 1.f);
        graph_line->setXValues(x_data);
      }

      if (m_x_autoscale && !m_graph_lines.back()->getXValues().empty()) {
        setAutoXScale();
      }
    }
  }
}

void Plot::updateXData(const std::vector<std::vector<float>>& x_data) {
  if (!x_data.empty()) {
    if (x_data.size() != m_graph_lines.size()) {
      throw std::invalid_argument(
          "Numbers of x_data vectors must be the same amount as y_data "
          "vectors. Make sure to set y_values first.");
    }

    std::size_t i = 0u;
    for (const auto& graph : m_graph_lines) {
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

SemiPlotX::SemiPlotX() { initialize(); }

std::unique_ptr<BaseGrid> LinearPlot::getGrid()
{
    return std::move(std::make_unique<Grid>());
}

std::unique_ptr<BaseGrid> SemiPlotX::getGrid()
{
    return std::move(std::make_unique<SemiLogXGrid>());
}

LinearPlot::LinearPlot() { initialize(); }

std::unique_ptr<scp::GraphLine> LinearPlot::getGraphLine() {
  return std::move(std::make_unique<scp::LinearGraphLine>());
}

std::unique_ptr<scp::GraphLine> SemiPlotX::getGraphLine() {
  return std::move(std::make_unique<scp::LogXGraphLine>());
}

}  // namespace scp
