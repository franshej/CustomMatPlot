#include <stdexcept>

#include "scp_lookandfeel.h"
#include "spl_label.h"
#include "spl_grid.h"
#include "spl_graph_line.h"
#include "scp_frame.h"
#include "scp_legend.h"

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
    const auto &single_data_vector =
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

PlotBase::~PlotBase() {
  m_grid->setLookAndFeel(nullptr);
  m_plot_label->setLookAndFeel(nullptr);
  m_frame->setLookAndFeel(nullptr);
  m_legend->setLookAndFeel(nullptr);
  for (auto& graph_line : m_graph_lines) {
    graph_line->setLookAndFeel(nullptr);
  }
}

void PlotBase::updateYLim(const float min, const float max) {
  for (auto& graph_line : m_graph_lines) {
    graph_line->setYLim(min, max);
  }
  if (m_grid != nullptr) {
    m_grid->setYLim(min, max);
  }
}

void PlotBase::updateXLim(const float min, const float max) {
  for (auto& graph_line : m_graph_lines) {
    graph_line->setXLim(min, max);
  }
  if (m_grid != nullptr) {
    m_grid->setXLim(min, max);
  }
}

void PlotBase::initialize() {
  m_grid = getGrid();
  m_plot_label = std::make_unique<PlotLabel>();
  m_frame = std::make_unique<Frame>();
  m_legend = std::make_unique<Legend>();

  lookAndFeelChanged();

  addChildComponent(m_legend.get());
  addAndMakeVisible(m_grid.get());
  addAndMakeVisible(m_plot_label.get());
  addAndMakeVisible(m_frame.get());
}

void PlotBase::setAutoXScale() {
  const auto [min, max] = findMinMaxValues(m_graph_lines, true);
  updateXLim(min, max);
}

void PlotBase::setAutoYScale() {
  const auto [min, max] = findMinMaxValues(m_graph_lines, false);
  updateYLim(min, max);
}

void PlotBase::xLim(const float min, const float max) {
  updateXLim(min, max);
  m_x_autoscale = false;
}

void PlotBase::yLim(const float min, const float max) {
  updateYLim(min, max);
  m_y_autoscale = false;
}

void PlotBase::Plot(const std::vector<std::vector<float>>& y_data,
                    const std::vector<std::vector<float>>& x_data,
                    ColourVector custom_graph_colours) {
  updateYData(y_data);
  if (!x_data.empty()) updateXData(x_data);

  if (m_lookandfeel) {
    auto lnf = static_cast<LookAndFeelMethods*>(m_lookandfeel_base);
    const auto graph_area = lnf->getGraphBounds(getBounds());
    repaint(graph_area);
  }
}

void PlotBase::setXLabel(const std::string& x_label) {
  m_plot_label->setXLabel(x_label);
}

void PlotBase::setXTickLabels(const std::vector<std::string>& x_labels) {
  m_grid->setXLabels(x_labels);
}

void PlotBase::setYTickLabels(const std::vector<std::string>& y_labels) {
  m_grid->setYLabels(y_labels);
}

void PlotBase::setXTicks(const std::vector<float>& x_ticks) {
  m_grid->setXTicks(x_ticks);
}

void PlotBase::setYTicks(const std::vector<float>& y_ticks) {
  m_grid->setYTicks(y_ticks);
}

void PlotBase::setYLabel(const std::string& y_label) {
  m_plot_label->setYLabel(y_label);
}

void PlotBase::setTitle(const std::string& title) { m_plot_label->setTitle(title); }

void PlotBase::makeGraphDashed(const std::vector<float>& dashed_lengths,
                               unsigned graph_index) {
  if (graph_index >= m_graph_lines.size()) {
    throw std::invalid_argument(
        "graph_index out of range, updateYData before 'makeGraphDashed'");
  }
  m_graph_lines[graph_index]->setDashedPath(dashed_lengths);
}

void PlotBase::gridON(const bool grid_on, const bool tiny_grid_on) {
  m_grid->setGridON(grid_on, tiny_grid_on);
}

void PlotBase::resized() {
  if (auto lnf = static_cast<LookAndFeelMethods*>(m_lookandfeel_base)) {
    const auto plot_area = lnf->getPlotBounds(getBounds());
    const auto graph_area = lnf->getGraphBounds(getBounds());

    if (m_grid) {
      m_grid->setBounds(plot_area);

      // (TODO) REMOVE !!!
      m_grid->setGridBounds(graph_area);
    }

    if (m_plot_label) m_plot_label->setBounds(plot_area);
    if (m_frame) m_frame->setBounds(graph_area);

    for (const auto& graph_line : m_graph_lines) {
      graph_line->setBounds(graph_area);
      graph_line->updateYGraphPoints();
      graph_line->updateXGraphPoints();
    }

    if (m_legend) {
      auto legend_bounds = m_legend->getBounds();
      const auto legend_postion =
          lnf->getLegendPosition(graph_area, legend_bounds);
      legend_bounds.setPosition(legend_postion);

      m_legend->setBounds(legend_bounds);
    }
  }
}

void PlotBase::paint(juce::Graphics& g) {}

void PlotBase::parentHierarchyChanged() { lookAndFeelChanged(); }

void PlotBase::lookAndFeelChanged() {
  if (auto* lnf = dynamic_cast<LookAndFeelMethods*>(&getLookAndFeel())) {
    m_lookandfeel = &getLookAndFeel();
    m_lookandfeel_default.reset();
    m_lookandfeel_base = lnf;
  } else {
    if (!m_lookandfeel_default)
      m_lookandfeel_default = std::make_unique<scp::PlotLookAndFeel>();
    m_lookandfeel = m_lookandfeel_default.get();
    m_lookandfeel_base = m_lookandfeel_default.get();
  }

  if (m_grid) m_grid->setLookAndFeel(m_lookandfeel);
  if (m_plot_label) m_plot_label->setLookAndFeel(m_lookandfeel);
  if (m_frame) m_frame->setLookAndFeel(m_lookandfeel);
  if (m_legend) m_legend->setLookAndFeel(m_lookandfeel);
  for (auto& graph_line : m_graph_lines) {
    graph_line->setLookAndFeel(m_lookandfeel);
  }
}

void PlotBase::updateYData(const std::vector<std::vector<float>>& y_data) {
  if (!y_data.empty()) {
    if (y_data.size() != m_graph_lines.size()) {
      m_graph_lines.resize(y_data.size());
      std::size_t i = 0u;
      for (auto& graph_line : m_graph_lines) {
        if (!graph_line) {
          if (auto lnf = static_cast<LookAndFeelMethods*>(m_lookandfeel_base)) {
            const auto colour_id = lnf->getColourFromGraphID(i);
            const auto graph_colour = lnf->findAndGetColourFromId(colour_id);

            graph_line = getGraphLine();
            graph_line->setColour(graph_colour);
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

void PlotBase::updateXData(const std::vector<std::vector<float>>& x_data) {
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

void PlotBase::setLegend(const StringVector& graph_descriptions) {
  if (m_lookandfeel_base && m_legend) {
    m_legend->setVisible(graph_descriptions.size() && m_graph_lines.size());
    m_legend->setDataSeries(&m_graph_lines);

    auto getDescriptionRef = [&]() -> const StringVector& {
      if (graph_descriptions.size() < m_graph_lines.size()) {
        auto descriptions = StringVector(m_graph_lines.size());
        descriptions = graph_descriptions;

        std::size_t i = 0u;
        std::for_each(descriptions.begin() + graph_descriptions.size(),
                      descriptions.end(), [&](auto& text) {
                        text = "Label " +
                               std::to_string(i + graph_descriptions.size());
                        i++;
                      });
        return descriptions;
      }
      return graph_descriptions;
    };

    const auto& graph_descriptions_ref = getDescriptionRef();

    const auto lnf = static_cast<LookAndFeelMethods*>(m_lookandfeel_base);
    const auto graph_bounds = lnf->getGraphBounds(getBounds());
    const auto bounds =
        lnf->getLegendBounds(graph_bounds, graph_descriptions_ref);

    m_legend->setBounds(bounds);
    m_legend->setLegend(graph_descriptions_ref);
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
