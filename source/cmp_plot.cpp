/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <stdexcept>

#include "cmp_datamodels.h"
#include "cmp_frame.h"
#include "cmp_graph_line.h"
#include "cmp_grid.h"
#include "cmp_label.h"
#include "cmp_legend.h"
#include "cmp_lookandfeel.h"
#include "cmp_trace.h"
#include "cmp_zoom.h"
#include "juce_core/system/juce_PlatformDefs.h"

namespace cmp {

static std::pair<float, float> findMinMaxValuesInGraphLines(
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

static std::pair<float, float> findMinMaxValuesInGraphLines(
    const std::vector<std::unique_ptr<cmp::GraphLine>>& graph_lines,
    const bool isXValue) noexcept {
  auto max_value = -std::numeric_limits<float>::max();
  auto min_value = std::numeric_limits<float>::max();

  for (const auto& graph : graph_lines) {
    const auto& single_data_vector =
        isXValue ? graph->getXValues() : graph->getYValues();

    if (!single_data_vector.empty()) {
      const auto& current_max = *std::max_element(single_data_vector.begin(),
                                                  single_data_vector.end());
      max_value = current_max > max_value ? current_max : max_value;
      const auto& current_min = *std::min_element(single_data_vector.begin(),
                                                  single_data_vector.end());
      min_value = current_min < min_value ? current_min : min_value;
    }
  }

  return {min_value, max_value};
}

template <bool is_point_data_point>
std::pair<juce::Point<float>, const GraphLine*> Plot::findNearestPoint(
    juce::Point<float> point, const GraphLine* graphline) {
  auto closest_point = juce::Point<float>(std::numeric_limits<float>::max(),
                                          std::numeric_limits<float>::max());

  juce::Point<float> closest_data_point, current_point, current_data_point;

  const auto graph_line_exsists =
      std::find_if(m_graph_lines.begin(), m_graph_lines.end(),
                   [&graphline](const auto& gl) {
                     return gl.get() == graphline;
                   }) != m_graph_lines.end();

  const GraphLine* nearest_graph_line{graph_line_exsists ? graphline : nullptr};

  if (!nearest_graph_line) {
    for (const auto& graph_line : m_graph_lines) {
      if constexpr (is_point_data_point) {
        current_data_point = current_point =
            graph_line->findClosestDataPointTo(point, false, false);

      } else {
        const auto [current_graph_point, data_point] =
            graph_line->findClosestGraphPointTo(point);

        current_point = current_graph_point;
        current_data_point = data_point;
      }

      if (point.getDistanceFrom(current_point) <
          point.getDistanceFrom(closest_point)) {
        closest_point = current_point;
        closest_data_point = current_data_point;
        nearest_graph_line = graph_line.get();
      }
    }
  } else if (graph_line_exsists) {
    if constexpr (is_point_data_point) {
      return {nearest_graph_line->findClosestDataPointTo(point),
              nearest_graph_line};
    }

    const auto [graph_point, data_point] =
        nearest_graph_line->findClosestGraphPointTo(point);
    closest_point = graph_point;
    closest_data_point = data_point;
  }

  return {closest_data_point, nearest_graph_line};
}

void Plot::resetLookAndFeelChildrens(juce::LookAndFeel* lookandfeel) {
  m_grid->setLookAndFeel(lookandfeel);
  m_plot_label->setLookAndFeel(lookandfeel);
  m_frame->setLookAndFeel(lookandfeel);
  m_legend->setLookAndFeel(lookandfeel);
  m_zoom->setLookAndFeel(lookandfeel);
  m_trace->setLookAndFeel(lookandfeel);

  for (const auto& graph_line : m_graph_lines) {
    graph_line->setLookAndFeel(lookandfeel);
  }

  for (const auto& spread : m_graph_spread_list) {
    spread->setLookAndFeel(lookandfeel);
  }
}

Plot::~Plot() { resetLookAndFeelChildrens(); }

enum RadioButtonIds { TraceZoomButtons = 1001 };

Plot::Plot(const Scaling x_scaling, const Scaling y_scaling)
    : m_x_scaling(x_scaling),
      m_y_scaling(y_scaling),
      m_plot_label(std::make_unique<PlotLabel>()),
      m_frame(std::make_unique<Frame>()),
      m_legend(std::make_unique<Legend>()),
      m_zoom(std::make_unique<Zoom>()),
      m_grid(std::make_unique<Grid>()),
      m_trace(std::make_unique<Trace>()),
      m_x_lim({0, 0}),
      m_y_lim({0, 0}),
      m_x_lim_default({0, 0}),
      m_y_lim_default({0, 0}),
      m_common_graph_params(m_graph_bounds, m_x_lim, m_y_lim, m_x_scaling,
                            m_y_scaling, m_downsampling_type) {
  lookAndFeelChanged();

  addAndMakeVisible(m_grid.get());
  addChildComponent(m_legend.get());
  addAndMakeVisible(m_zoom.get());
  addAndMakeVisible(m_plot_label.get());
  addAndMakeVisible(m_frame.get());

  m_legend->setAlwaysOnTop(true);
  m_zoom->toBehind(m_legend.get());
  m_grid->toBack();

  m_grid->onGridLabelLengthChanged = [this](cmp::Grid* grid) {
    this->resizeChilderns();
  };

  m_legend->onNumberOfDescriptionsChanged = [this](const auto& desc) {
    if (auto lnf = static_cast<LookAndFeelMethods*>(m_lookandfeel)) {
      const auto legend_bounds = lnf->getLegendBounds(m_graph_bounds, desc);

      m_legend->setBounds(legend_bounds);
    }
  };

  m_trace->onTracePointChanged = [this](const auto* trace_point,
                                        const auto prev_data,
                                        const auto new_data) {
    if (onTraceValueChange) {
      onTraceValueChange(trace_point, prev_data, new_data);
    }
  };
}

void Plot::updateXLim(const Lim_f& new_x_lim) {
  if (new_x_lim.min > new_x_lim.max) UNLIKELY
  throw std::invalid_argument("Min value must be lower than max value.");

  if ((abs(new_x_lim.max - new_x_lim.min) <
       std::numeric_limits<float>::epsilon()))
    UNLIKELY {
      m_x_lim.min = new_x_lim.min - 1;
      m_x_lim.max = new_x_lim.max + 1;
    }

  if (m_x_scaling == Scaling::logarithmic && new_x_lim.isMinOrMaxZero())
    UNLIKELY {
      throw std::invalid_argument(
          "The min/max x-value is zero or 'xLim' has been called with a zero "
          "value. 10log(0) = -inf");
    }

  if (new_x_lim && new_x_lim != m_x_lim) {
    m_x_lim = new_x_lim;

    if (m_y_lim) {
      updateGridGraphsTrace();
    }
  }
}

void Plot::updateYLim(const Lim_f& new_y_lim) {
  if (new_y_lim.min > new_y_lim.max) UNLIKELY
  throw std::invalid_argument("Min value must be lower than max value.");

  if (abs(new_y_lim.max - new_y_lim.min) <
      std::numeric_limits<float>::epsilon())
    UNLIKELY {
      m_y_lim.min = new_y_lim.min - 1;
      m_y_lim.max = new_y_lim.max + 1;
    }

  if (m_y_scaling == Scaling::logarithmic && new_y_lim.isMinOrMaxZero())
    UNLIKELY {
      throw std::invalid_argument(
          "The min/max y-value is zero or 'yLim' has been called with a zero "
          "value. 10log(0) = -inf");
    }

  if (new_y_lim && m_y_lim != new_y_lim) {
    m_y_lim = new_y_lim;

    if (m_x_lim) {
      updateGridGraphsTrace();
    }
  }
}

void Plot::updateGridGraphsTrace() {
  if (!m_graph_bounds.isEmpty()) {
    m_grid->updateGrid(m_common_graph_params);
    m_trace->updateTracePointsBoundsFrom(m_common_graph_params);

    for (const auto& graph_line : m_graph_lines) {
      graph_line->updateXGraphPoints(m_common_graph_params);
      graph_line->updateYGraphPoints(m_common_graph_params);
    }
  }
}

void cmp::Plot::updateTracePointsForNewGraphData() {
  for (const auto& graph_line : m_graph_lines) {
    m_trace->updateTracePointsAssociatedWith(graph_line.get());
  }

  m_trace->updateTracePointsBoundsFrom(m_common_graph_params);

  if (m_legend->isVisible()) {
    m_legend->updateLegends(m_graph_lines);
  }
}

static auto getLimOffset(const auto min, const auto max, const auto scaling) {
  auto new_min = min;

  if (scaling == Scaling::linear) {
    const auto diff = (max - min) / decltype(min)(20);

    const auto new_min_offset = min - diff;
    const auto new_max_offset = max + diff;

    return Lim<decltype(new_min)>(new_min_offset, new_max_offset);
  }

  return Lim<decltype(new_min)>(min, max);
}

void Plot::setAutoXScale() {
  const auto [min, max] = findMinMaxValuesInGraphLines(m_graph_lines, true);

  m_x_lim_default = getLimOffset(min, max, m_x_scaling);

  updateXLim(m_x_lim_default);
}

void Plot::setAutoYScale() {
  const auto [min, max] = findMinMaxValuesInGraphLines(m_graph_lines, false);

  m_y_lim_default = getLimOffset(min, max, m_y_scaling);

  updateYLim(m_y_lim_default);
}

void Plot::xLim(const float min, const float max) {
  updateXLim({min, max});
  m_x_lim_default = {min, max};
  m_x_autoscale = false;
}

void Plot::yLim(const float min, const float max) {
  updateYLim({min, max});
  m_y_lim_default = {min, max};
  m_y_autoscale = false;
}

void Plot::plot(const std::vector<std::vector<float>>& y_data,
                const std::vector<std::vector<float>>& x_data,
                const GraphAttributeList& graph_attributes) {
  updateYData(y_data, graph_attributes);
  updateXData(x_data);

  updateTracePointsForNewGraphData();

  repaint();
}

void Plot::realTimePlot(const std::vector<std::vector<float>>& y_data) {
  updateYData(y_data, {});

  if (m_graph_lines[0]->getXValues().empty()) UNLIKELY {
      updateXData({});
    }

  updateTracePointsForNewGraphData();

  repaint(m_graph_bounds);
}

void Plot::fillBetween(
    const std::vector<GraphSpreadIndex>& graph_spread_indices,
    const std::vector<juce::Colour>& fill_area_colours) {
  m_graph_spread_list.resize(graph_spread_indices.size());
  auto graph_spread_it = m_graph_spread_list.begin();

  for (const auto& spread_index : graph_spread_indices) {
    if (std::max(spread_index.first_graph, spread_index.second_graph) >=
        m_graph_lines.size()) {
      throw std::range_error("Spread index out of range.");
    }
    const auto first_graph = m_graph_lines[spread_index.first_graph].get();
    const auto second_graph = m_graph_lines[spread_index.second_graph].get();

    auto it_colour = fill_area_colours.begin();

    const auto colour = it_colour != fill_area_colours.end()
                            ? *it_colour++
                            : first_graph->getColour();

    auto& graph_spread = *graph_spread_it++;

    if (!graph_spread) {
      graph_spread =
          std::make_unique<GraphSpread>(first_graph, second_graph, colour);

      graph_spread->setBounds(m_graph_bounds);

      graph_spread->setLookAndFeel(m_lookandfeel);

      addAndMakeVisible(graph_spread.get());

      graph_spread->toBehind(m_zoom.get());
    } else {
      graph_spread->m_lower_bound = first_graph;
      graph_spread->m_upper_bound = second_graph;
      graph_spread->m_spread_colour = colour;
      graph_spread->setBounds(m_graph_bounds);
    }
  }
}

void cmp::Plot::setDownsamplingType(
    const DownsamplingType downsampling_type) noexcept {
  m_downsampling_type = downsampling_type;

  updateGridGraphsTrace();
}

static auto jassetLogLimBelowZero(const auto scaling, const auto lim) {
  const auto zero = decltype(lim.min)(0);

  // You are trying to use a negative limit but the axis is logarithmic. Don't
  // do that. Use ylim or xlim to set limits above 0. Then call 'setScaling()'
  // if that is used.
  jassert(!(scaling == Scaling::logarithmic &&
            (lim.min <= zero || lim.max <= zero)));
}

void Plot::setScaling(const Scaling x_scaling,
                      const Scaling y_scaling) noexcept {
  if (x_scaling != m_x_scaling || y_scaling != m_y_scaling) {
    jassetLogLimBelowZero(x_scaling, m_x_lim_default);
    jassetLogLimBelowZero(y_scaling, m_y_lim_default);

    m_x_scaling = x_scaling;
    m_y_scaling = y_scaling;

    resetLookAndFeelChildrens();
    m_lookandfeel_default.reset(nullptr);
    lookAndFeelChanged();

    updateGridGraphsTrace();
  }
}

void Plot::setXLabel(const std::string& x_label) {
  m_plot_label->setXLabel(x_label);

  resizeChilderns();
}

void Plot::setXTickLabels(const std::vector<std::string>& x_labels) {
  m_grid->setXLabels(x_labels);

  if (!m_graph_bounds.isEmpty()) m_grid->updateGrid(m_common_graph_params);
}

void Plot::setYTickLabels(const std::vector<std::string>& y_labels) {
  m_grid->setYLabels(y_labels);

  if (!m_graph_bounds.isEmpty()) m_grid->updateGrid(m_common_graph_params);
}

void Plot::setXTicks(const std::vector<float>& x_ticks) {
  m_grid->setXTicks(x_ticks);

  if (!m_graph_bounds.isEmpty()) m_grid->updateGrid(m_common_graph_params);
}

void Plot::setYTicks(const std::vector<float>& y_ticks) {
  m_grid->setYTicks(y_ticks);

  if (!m_graph_bounds.isEmpty()) m_grid->updateGrid(m_common_graph_params);
}

void Plot::setYLabel(const std::string& y_label) {
  m_plot_label->setYLabel(y_label);

  resizeChilderns();
}

void Plot::setTitle(const std::string& title) {
  m_plot_label->setTitle(title);

  resizeChilderns();
}

void Plot::setTracePoint(const juce::Point<float>& trace_point_coordinate) {
  const auto [closest_data_point, nearest_graph_line] =
      findNearestPoint<true>(trace_point_coordinate);

  m_trace->addOrRemoveTracePoint(closest_data_point, nearest_graph_line);

  m_trace->updateTracePointsBoundsFrom(m_common_graph_params);

  m_trace->addAndMakeVisibleTo(this);
}

void Plot::gridON(const bool grid_on, const bool tiny_grid_on) {
  m_grid->setGridON(grid_on, tiny_grid_on);
}

void Plot::clearTracePoints() noexcept { m_trace->clear(); }

void Plot::resizeChilderns() {
  if (auto lnf = static_cast<LookAndFeelMethods*>(m_lookandfeel)) {
    const auto plot_area = lnf->getPlotBounds(getBounds());
    const auto graph_bounds = lnf->getGraphBounds(getBounds(), this);

    if (!graph_bounds.isEmpty() && m_graph_bounds != graph_bounds) {
      m_graph_bounds = graph_bounds;

      if (m_grid) {
        m_grid->setGridBounds(graph_bounds);
        m_grid->setBounds(plot_area);
      }

      if (m_plot_label) m_plot_label->setBounds(plot_area);

      constexpr auto margin_for_1px_outside = 1;
      const juce::Rectangle<int> frame_bound = {
          graph_bounds.getX(), graph_bounds.getY(),
          graph_bounds.getWidth() + margin_for_1px_outside,
          graph_bounds.getHeight() + margin_for_1px_outside};

      if (m_frame) m_frame->setBounds(frame_bound);

      if (m_zoom) m_zoom->setBounds(graph_bounds);

      for (const auto& graph_line : m_graph_lines) {
        graph_line->setBounds(graph_bounds);
      }

      for (const auto& spread : m_graph_spread_list) {
        spread->setBounds(graph_bounds);
      }

      if (m_legend) {
        auto legend_bounds = m_legend->getBounds();
        const auto legend_postion =
            lnf->getLegendPosition(graph_bounds, legend_bounds);
        legend_bounds.setPosition(legend_postion);

        m_legend->setBounds(legend_bounds);
      }

      updateGridGraphsTrace();
    }
  }
}

void Plot::resized() { resizeChilderns(); }

void Plot::paint(juce::Graphics& g) {
  if (m_lookandfeel) {
    auto lnf = static_cast<LookAndFeelMethods*>(m_lookandfeel);

    lnf->drawBackground(g, m_common_graph_params.graph_bounds);
  }
}

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
    return lnf;
  }
  return nullptr;
}

void Plot::setLookAndFeel(PlotLookAndFeel* look_and_feel) {
  juce::LookAndFeel* lnf{nullptr};

  if ((lnf = castUserLookAndFeel<Scaling::linear, Scaling::linear>(
           look_and_feel))) {
  } else if ((lnf = castUserLookAndFeel<Scaling::logarithmic, Scaling::linear>(
                  look_and_feel))) {
  } else if ((lnf = castUserLookAndFeel<Scaling::logarithmic,
                                        Scaling::logarithmic>(look_and_feel))) {
  } else if ((lnf = castUserLookAndFeel<Scaling::linear, Scaling::logarithmic>(
                  look_and_feel))) {
  }

  resetLookAndFeelChildrens();

  this->juce::Component::setLookAndFeel(lnf);

  resizeChilderns();
}

std::unique_ptr<Plot::LookAndFeelMethods> Plot::getDefaultLookAndFeel() {
  if (m_x_scaling == Scaling::logarithmic &&
      m_y_scaling == Scaling::logarithmic) {
    return std::make_unique<cmp::PlotLookAndFeelDefault<
        Scaling::logarithmic, Scaling::logarithmic>>();
  } else if (m_x_scaling == Scaling::logarithmic) {
    return std::make_unique<
        cmp::PlotLookAndFeelDefault<Scaling::logarithmic, Scaling::linear>>();
  } else if (m_y_scaling == Scaling::logarithmic) {
    return std::make_unique<
        cmp::PlotLookAndFeelDefault<Scaling::linear, Scaling::logarithmic>>();
  } else {
    return std::make_unique<
        cmp::PlotLookAndFeelDefault<Scaling::linear, Scaling::linear>>();
  }
}

void Plot::lookAndFeelChanged() {
  if (auto* lnf = dynamic_cast<LookAndFeelMethods*>(&getLookAndFeel())) {
    m_lookandfeel = &getLookAndFeel();
    m_lookandfeel_default.reset();
  } else {
    if (!m_lookandfeel_default) {
      m_lookandfeel_default = getDefaultLookAndFeel();
    }
    m_lookandfeel = m_lookandfeel_default.get();
  }

  resetLookAndFeelChildrens(m_lookandfeel);
}

void Plot::updateYData(const std::vector<std::vector<float>>& y_data,
                       const GraphAttributeList& graph_attribute_list) {
  if (!y_data.empty()) {
    if (y_data.size() != m_graph_lines.size()) UNLIKELY {
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
            graph_line->setBounds(m_graph_bounds);

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

    if (m_y_autoscale && !m_y_lim) UNLIKELY {
        setAutoYScale();
      }

    if (!graph_attribute_list.empty()) {
      auto it_gal = graph_attribute_list.begin();
      for (const auto& graph_line : m_graph_lines) {
        if (it_gal != graph_attribute_list.end()) {
          graph_line->setGraphAttribute(*it_gal);
        }

        ++it_gal;
      }
    }

    for (const auto& graph_line : m_graph_lines) {
      graph_line->updateYGraphPoints(m_common_graph_params);
    }
  }
}

void Plot::updateXData(const std::vector<std::vector<float>>& x_data) {
  std::vector<std::vector<float>> x_data_gen;
  const auto* data = &x_data;

  if (x_data.size() != m_graph_lines.size()) {
    x_data_gen.resize(m_graph_lines.size());

    data = &x_data_gen;
  }

  bool trigger_y_data_update = false;

  std::size_t i = 0u;
  for (const auto& graph : m_graph_lines) {
    const auto y_graph_length = graph->getYValues().size();
    const auto x_graph_length = (*data)[i].size();
    std::vector<float> x_data_auto;

    const auto* current_x_data = &(*data)[i];

    if (x_graph_length == std::size_t(0u)) UNLIKELY {
        x_data_auto.resize(y_graph_length);

        std::iota(x_data_auto.begin(), x_data_auto.end(), 1.0f);

        current_x_data = &x_data_auto;

        trigger_y_data_update = true;
      }
    else if (y_graph_length != x_graph_length) {
      throw std::invalid_argument(
          "Invalid vector length.\nLength of y vector at index " +
          std::to_string(i) + " is " + std::to_string(y_graph_length) +
          "\nLength of x vector at index " + std::to_string(i) + " is " +
          std::to_string(y_graph_length));
    }

    graph->setXValues(*current_x_data);
    i++;
  }

  if (m_x_autoscale) {
    setAutoXScale();
  }

  for (const auto& graph_line : m_graph_lines) {
    graph_line->updateXGraphPoints(m_common_graph_params);
    if (trigger_y_data_update) UNLIKELY {
        graph_line->updateYGraphPoints(m_common_graph_params);
      }
  }
}

void Plot::setLegend(const StringVector& graph_descriptions) {
  if (m_lookandfeel && m_legend) {
    const auto lnf = static_cast<LookAndFeelMethods*>(m_lookandfeel);

    m_legend->setVisible(true);

    m_legend->setLegend(graph_descriptions);
    m_legend->updateLegends(m_graph_lines);
  }
}

void Plot::mouseDown(const juce::MouseEvent& event) {
  if (isVisible()) {
    if (m_zoom.get() == event.eventComponent) {
      if (event.mods.isRightButtonDown()) {
        updateXLim(m_x_lim_default);
        updateYLim(m_y_lim_default);

        updateGridGraphsTrace();

        repaint();
      } else if (m_legend.get() == event.eventComponent) {
        m_comp_dragger.startDraggingComponent(event.eventComponent, event);
      }
    }

    if (event.getNumberOfClicks() > 1) {
      const auto component_pos =
          event.eventComponent->getBounds().getPosition();

      const auto mouse_pos =
          (event.getPosition() + component_pos - m_graph_bounds.getPosition())
              .toFloat();

      const auto [closest_data_point, nearest_graph_line] =
          findNearestPoint(mouse_pos, nullptr);

      m_trace->addOrRemoveTracePoint(closest_data_point, nearest_graph_line);
      m_trace->updateTracePointsBoundsFrom(m_common_graph_params);
      m_trace->addAndMakeVisibleTo(this);
    }
  }
}

void Plot::mouseDrag(const juce::MouseEvent& event) {
  if (isVisible()) {
    if (m_legend.get() == event.eventComponent) {
      m_comp_dragger.dragComponent(event.eventComponent, event, nullptr);
    } else if (m_zoom.get() == event.eventComponent &&
               event.getDistanceFromDragStart() > 4 &&
               event.getNumberOfClicks() == 1) {
      if (!m_zoom->isStartPosSet()) {
        m_zoom->setStartPosition(event.getPosition());
      } else {
        m_zoom->setEndPosition(event.getPosition());

        m_zoom->repaint();
      }
    } else if (m_trace->isComponentTracePoint(event.eventComponent) &&
               event.getNumberOfClicks() == 1) {
      auto bounds = event.eventComponent->getBounds();

      const auto mouse_pos =
          bounds.getPosition() - m_graph_bounds.getPosition() +
          event.getEventRelativeTo(event.eventComponent).getPosition();

      const auto* associated_graph_line =
          m_trace->getAssociatedGraphLine(event.eventComponent);

      const auto [closest_data_point, nearest_graph_line] =
          findNearestPoint(mouse_pos.toFloat(), associated_graph_line);

      m_trace->setDataValueFor(event.eventComponent, closest_data_point,
                               m_common_graph_params);

    } else if (m_trace->isComponentTraceLabel(event.eventComponent)) {
      auto bounds = event.eventComponent->getBounds();

      const auto mouse_pos =
          bounds.getPosition() +
          event.getEventRelativeTo(event.eventComponent).getPosition();

      if (m_trace->setCornerPositionForLabelAssociatedWith(event.eventComponent,
                                                           mouse_pos)) {
        m_trace->updateSingleTracePointBoundsFrom(event.eventComponent,
                                                  m_common_graph_params);
      }
    }
  }
}

void Plot::mouseUp(const juce::MouseEvent& event) {
  if (isVisible()) {
    if (m_zoom.get() == event.eventComponent && m_zoom->isStartPosSet() &&
        !event.mods.isRightButtonDown()) {
      const auto local_graph_bounds = getLocalBoundsFrom<float>(m_graph_bounds);

      const auto start_pos = m_zoom->getStartPosition();
      const auto end_pos = m_zoom->getEndPosition();

      const auto x_min = getXFromXCoordinate(
          float(start_pos.getX()), local_graph_bounds, m_x_lim, m_x_scaling);
      const auto x_max = getXFromXCoordinate(
          float(end_pos.getX()), local_graph_bounds, m_x_lim, m_x_scaling);

      const auto y_min = getYFromYCoordinate(
          float(start_pos.getY()), local_graph_bounds, m_y_lim, m_y_scaling);
      const auto y_max = getYFromYCoordinate(
          float(end_pos.getY()), local_graph_bounds, m_y_lim, m_y_scaling);

      updateXLim({std::min(x_min, x_max), std::max(x_min, x_max)});
      updateYLim({std::min(y_min, y_max), std::max(y_min, y_max)});

      updateGridGraphsTrace();
      m_zoom->reset();

      repaint();
    }
  }
}

}  // namespace cmp
