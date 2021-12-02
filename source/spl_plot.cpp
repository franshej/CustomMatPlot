#include <stdexcept>

#include "scp_frame.h"
#include "scp_legend.h"
#include "scp_lookandfeel.h"
#include "scp_zoom.h"
#include "spl_graph_line.h"
#include "spl_grid.h"
#include "spl_label.h"

namespace scp {

static inline [[nodiscard]] std::pair<float, float>
findMinMaxValuesInGraphLines(
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

static inline [[nodiscard]] std::pair<float, float>
findMinMaxValuesInGraphLines(
    const std::vector<std::unique_ptr<scp::GraphLine>>& graph_lines,
    const bool isXValue) noexcept {
  auto max_value = -std::numeric_limits<float>::max();
  auto min_value = std::numeric_limits<float>::max();

  for (const auto& graph : graph_lines) {
    const auto& single_data_vector =
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

void Plot::resetLookAndFeel() {
  m_grid->setLookAndFeel(nullptr);
  m_plot_label->setLookAndFeel(nullptr);
  m_frame->setLookAndFeel(nullptr);
  m_legend->setLookAndFeel(nullptr);
  m_zoom->setLookAndFeel(nullptr);
  for (auto& graph_line : m_graph_lines) {
    graph_line->setLookAndFeel(nullptr);
  }
}

Plot::~Plot() { resetLookAndFeel(); }

Plot::Plot(const Scaling x_scaling, const Scaling y_scaling)
    : m_x_scaling(x_scaling), m_y_scaling(y_scaling) {
  initialize();
}

void Plot::updateYLim(const float min, const float max) {
  m_y_lim = {min, max};
  for (auto& graph_line : m_graph_lines) {
    graph_line->setYLim(min, max);
  }
  if (m_grid != nullptr) {
    m_grid->setYLim(min, max);
  }
}

void Plot::updateXLim(const float min, const float max) {
  m_x_lim = {min, max};
  for (auto& graph_line : m_graph_lines) {
    graph_line->setXLim(min, max);
  }
  if (m_grid != nullptr) {
    m_grid->setXLim(min, max);
  }
}

void Plot::updateGridAndGraphs() {
  m_grid->updateGrid();

  for (const auto& graph_line : m_graph_lines) {
    graph_line->updateXGraphPoints();
    graph_line->updateYGraphPoints();
  }
}

void Plot::initialize() {
  m_plot_label = std::make_unique<PlotLabel>();
  m_frame = std::make_unique<Frame>();
  m_legend = std::make_unique<Legend>();
  m_zoom = std::make_unique<Zoom>();
  m_grid = std::make_unique<Grid>();

  lookAndFeelChanged();

  addAndMakeVisible(m_grid.get());
  addChildComponent(m_legend.get());
  addAndMakeVisible(m_zoom.get());
  addAndMakeVisible(m_plot_label.get());
  addAndMakeVisible(m_frame.get());

  m_legend->setAlwaysOnTop(true);
  m_zoom->toBehind(m_legend.get());
  m_grid->toBack();
}

void Plot::setAutoXScale() {
  const auto [min, max] = findMinMaxValuesInGraphLines(m_graph_lines, true);
  m_x_lim_default = {min, max};
  updateXLim(min, max);
}

void Plot::setAutoYScale() {
  const auto [min, max] = findMinMaxValuesInGraphLines(m_graph_lines, false);
  m_y_lim_default = {min, max};
  updateYLim(min, max);
}

void Plot::xLim(const float min, const float max) {
  updateXLim(min, max);
  m_x_lim_default = {min, max};
  m_x_autoscale = false;
}

void Plot::yLim(const float min, const float max) {
  updateYLim(min, max);
  m_y_lim_default = {min, max};
  m_y_autoscale = false;
}

void Plot::plot(const std::vector<std::vector<float>>& y_data,
                const std::vector<std::vector<float>>& x_data,
                ColourVector custom_graph_colours) {
  updateYData(y_data);
  if (!x_data.empty()) updateXData(x_data);

  if (m_lookandfeel) {
    auto lnf = static_cast<LookAndFeelMethods*>(m_lookandfeel);
    const auto graph_area = lnf->getGraphBounds(getBounds());
    repaint(graph_area);
  }
}

void Plot::setXLabel(const std::string& x_label) {
  m_plot_label->setXLabel(x_label);
}

void Plot::setXTickLabels(const std::vector<std::string>& x_labels) {
  m_grid->setXLabels(x_labels);
}

void Plot::setYTickLabels(const std::vector<std::string>& y_labels) {
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
  if (auto lnf = static_cast<LookAndFeelMethods*>(m_lookandfeel)) {
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
      graph_line->updateXGraphPoints();
      graph_line->updateYGraphPoints();
    }

    if (m_legend) {
      auto legend_bounds = m_legend->getBounds();
      const auto legend_postion =
          lnf->getLegendPosition(graph_area, legend_bounds);
      legend_bounds.setPosition(legend_postion);

      m_legend->setBounds(legend_bounds);
    }

    if (m_zoom) {
      m_zoom->setBounds(graph_area);
    }
  }
}

void Plot::paint(juce::Graphics& g) {}

void Plot::parentHierarchyChanged() {
  getParentComponent()->addMouseListener(this, true);
  lookAndFeelChanged();
}

template <Scaling x_scaling, Scaling y_scaling>
juce::LookAndFeel* Plot::castUserLookAndFeel(
    PlotLookAndFeel* user_look_and_feel) {
  if (m_x_scaling != x_scaling || m_y_scaling != y_scaling) {
  } else if (auto* lnf =
                 dynamic_cast<PlotLookAndFeelDefault<x_scaling, y_scaling>*>(
                     user_look_and_feel)) {
    resetLookAndFeel();
    return lnf;
  }
  return nullptr;
}

void Plot::setLookAndFeel(PlotLookAndFeel* look_and_feel) {
  juce::LookAndFeel* lnf{nullptr};

  if (lnf = castUserLookAndFeel<Scaling::linear, Scaling::linear>(
          look_and_feel)) {
  } else if (lnf = castUserLookAndFeel<Scaling::logarithmic, Scaling::linear>(
                 look_and_feel)) {
  } else if (lnf = castUserLookAndFeel<Scaling::logarithmic,
                                       Scaling::logarithmic>(look_and_feel)) {
  } else if (lnf = castUserLookAndFeel<Scaling::linear, Scaling::logarithmic>(
                 look_and_feel)) {
  }
  this->juce::Component::setLookAndFeel(lnf);
}

void Plot::lookAndFeelChanged() {
  if (&getLookAndFeel() == nullptr) {
    m_lookandfeel_default.reset();
    m_lookandfeel = nullptr;
    resetLookAndFeel();
  } else if (auto* lnf = dynamic_cast<LookAndFeelMethods*>(&getLookAndFeel())) {
    m_lookandfeel = &getLookAndFeel();
    m_lookandfeel_default.reset();
  } else {
    if (!m_lookandfeel_default)
      if (m_x_scaling == Scaling::logarithmic &&
          m_y_scaling == Scaling::logarithmic) {
        m_lookandfeel_default = std::make_unique<scp::PlotLookAndFeelDefault<
            Scaling::logarithmic, Scaling::logarithmic>>();
      } else if (m_x_scaling == Scaling::logarithmic) {
        m_lookandfeel_default =
            std::make_unique<scp::PlotLookAndFeelDefault<Scaling::logarithmic,
                                                         Scaling::linear>>();
      } else if (m_y_scaling == Scaling::logarithmic) {
        m_lookandfeel_default = std::make_unique<scp::PlotLookAndFeelDefault<
            Scaling::linear, Scaling::logarithmic>>();
      } else {
        m_lookandfeel_default = std::make_unique<
            scp::PlotLookAndFeelDefault<Scaling::linear, Scaling::linear>>();
      }
    m_lookandfeel = m_lookandfeel_default.get();
  }

  if (m_grid) m_grid->setLookAndFeel(m_lookandfeel);
  if (m_plot_label) m_plot_label->setLookAndFeel(m_lookandfeel);
  if (m_frame) m_frame->setLookAndFeel(m_lookandfeel);
  if (m_legend) m_legend->setLookAndFeel(m_lookandfeel);
  if (m_zoom) m_zoom->setLookAndFeel(m_lookandfeel);
  if (m_grid) m_grid->setLookAndFeel(m_lookandfeel);
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
        if (!graph_line && m_lookandfeel) {
          auto lnf = static_cast<LookAndFeelMethods*>(m_lookandfeel);
          const auto colour_id = lnf->getColourFromGraphID(i);
          const auto graph_colour = lnf->findAndGetColourFromId(colour_id);

          graph_line = std::make_unique<GraphLine>();
          graph_line->setColour(graph_colour);
          graph_line->setLookAndFeel(m_lookandfeel);

          const auto graph_area = lnf->getGraphBounds(getBounds());
          graph_line->setBounds(graph_area);

          addAndMakeVisible(graph_line.get());
          graph_line->toBehind(m_zoom.get());
          i++;
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
        auto x_data = std::vector<float>(graph_line->getYValues().size());
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

void Plot::setLegend(const StringVector& graph_descriptions) {
  if (m_lookandfeel && m_legend) {
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

    const auto graph_descriptions_ref = getDescriptionRef();

    const auto lnf = static_cast<LookAndFeelMethods*>(m_lookandfeel);
    const auto graph_bounds = lnf->getGraphBounds(getBounds());
    const auto bounds =
        lnf->getLegendBounds(graph_bounds, graph_descriptions_ref);

    m_legend->setBounds(bounds);
    m_legend->setLegend(graph_descriptions_ref);
  }
}

void Plot::mouseDown(const juce::MouseEvent& event) {
  if (isVisible()) {
    if (m_zoom.get() == event.eventComponent) {
      if (event.mods.isRightButtonDown()) {
        updateXLim(m_x_lim_default.min, m_x_lim_default.max);
        updateYLim(m_y_lim_default.min, m_y_lim_default.max);

        updateGridAndGraphs();

        repaint();
      } else {
        m_zoom->setStartPosition(event.getPosition());
      }
    }
  }
}

void Plot::mouseDrag(const juce::MouseEvent& event) {
  if (isVisible()) {
    if (m_legend.get() == event.eventComponent) {
      m_comp_dragger.dragComponent(event.eventComponent, event, nullptr);
    } else if (m_zoom.get() == event.eventComponent) {
      const auto lnf = static_cast<LookAndFeelMethods*>(m_lookandfeel);
      const auto graph_bounds = lnf->getGraphBounds(getBounds());
      DBG("x, y value: [ "
          << std::to_string(getXFromXCoordinate(event.position.getX(), 0,
                                                graph_bounds.getWidth(),
                                                m_x_lim, m_x_scaling))
          << ", "
          << std::to_string(getYFromYCoordinate(event.position.getY(), 0,
                                                graph_bounds.getHeight(),
                                                m_y_lim, m_y_scaling))
          << " ]");
      DBG("Pos: " << event.position.toString());
      m_zoom->setEndPosition(event.getPosition());
      m_zoom->repaint();
    }
  }
}

void Plot::mouseUp(const juce::MouseEvent& event) {
  if (isVisible()) {
    if (m_zoom.get() == event.eventComponent &&
        !event.mods.isRightButtonDown()) {
      const auto lnf = static_cast<LookAndFeelMethods*>(m_lookandfeel);
      const auto graph_bounds = lnf->getGraphBounds(getBounds());

      const auto start_pos = m_zoom->getStartPosition();
      const auto end_pos = m_zoom->getEndPosition();

      const auto x_min = getXFromXCoordinate(
          start_pos.getX(), 0, graph_bounds.getWidth(), m_x_lim, m_x_scaling);
      const auto x_max = getXFromXCoordinate(
          end_pos.getX(), 0, graph_bounds.getWidth(), m_x_lim, m_x_scaling);

      const auto y_min = getYFromYCoordinate(
          start_pos.getY(), 0, graph_bounds.getHeight(), m_y_lim, m_y_scaling);
      const auto y_max = getYFromYCoordinate(
          end_pos.getY(), 0, graph_bounds.getHeight(), m_y_lim, m_y_scaling);

      updateXLim(std::min(x_min, x_max), std::max(x_min, x_max));
      updateYLim(std::min(y_min, y_max), std::max(y_min, y_max));

      updateGridAndGraphs();
      m_zoom->reset();

      repaint();
    }
  }
}
}  // namespace scp
