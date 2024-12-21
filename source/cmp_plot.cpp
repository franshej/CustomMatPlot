/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "cmp_plot.h"

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>

#include "cmp_datamodels.h"
#include "cmp_frame.h"
#include "cmp_graph_area.h"
#include "cmp_graph_line.h"
#include "cmp_grid.h"
#include "cmp_label.h"
#include "cmp_legend.h"
#include "cmp_lookandfeel.h"
#include "cmp_trace.h"
#include "cmp_utils.h"
#include "juce_core/system/juce_PlatformDefs.h"

namespace cmp {

static std::pair<float, float> findMinMaxValuesInGraphLines(
    const std::vector<std::unique_ptr<cmp::GraphLine>>& graph_lines,
    const bool isXValue) noexcept {
  auto max_value = -std::numeric_limits<float>::max();
  auto min_value = std::numeric_limits<float>::max();

  for (const auto& graph : graph_lines) {
    const auto& values = isXValue ? graph->getXData() : graph->getYData();

    if (!values.empty()) {
      const auto& current_max = *std::max_element(values.begin(), values.end());
      max_value = current_max > max_value ? current_max : max_value;
      const auto& current_min = *std::min_element(values.begin(), values.end());
      min_value = current_min < min_value ? current_min : min_value;
    }
  }

  return {min_value, max_value};
}

template <class ValueType>
static Lim<ValueType> createLimsIfTheSame(const Lim<ValueType>& lims) noexcept {
  if (lims.min == lims.max) {
    const auto one = ValueType(1);
    return {lims.min - one, lims.max + one};
  }
  return lims;
}

template <bool is_point_data_point>
std::tuple<size_t, const GraphLine*> Plot::findNearestPoint(
    juce::Point<float> point, const GraphLine* graphline) {
  auto closest_point = juce::Point<float>(std::numeric_limits<float>::max(),
                                          std::numeric_limits<float>::max());

  juce::Point<float> closest_data_point, current_point, current_data_point;
  size_t data_point_index{0};
  size_t closest_data_point_index{0};

  const auto graph_line_exsists =
      std::find_if(m_graph_lines->begin(), m_graph_lines->end(),
                   [&graphline](const auto& gl) {
                     return gl.get() == graphline;
                   }) != m_graph_lines->end();

  const GraphLine* nearest_graph_line{graph_line_exsists ? graphline : nullptr};

  if (!nearest_graph_line) {
    for (const auto& graph_line : *m_graph_lines) {
      auto current_data_point_index = size_t{0};
      if constexpr (is_point_data_point) {
        const auto [data_point, data_point_index_temp] =
            graph_line->findClosestDataPointTo(point, false, false);

        current_data_point = current_point = data_point;
        data_point_index = data_point_index_temp;
      } else {
        const auto [current_pixel_point, data_point, data_point_index_temp] =
            graph_line->findClosestPixelPointTo(point);

        current_point = current_pixel_point;
        current_data_point = data_point;
        current_data_point_index = data_point_index_temp;
      }

      if (point.getDistanceFrom(current_point) <
          point.getDistanceFrom(closest_point)) {
        closest_point = current_point;
        closest_data_point = current_data_point;
        nearest_graph_line = graph_line.get();
        data_point_index = current_data_point_index;
      }
    }
  } else if (graph_line_exsists) {
    if constexpr (is_point_data_point) {
      const auto [data_point, data_point_index_temp] =
          nearest_graph_line->findClosestDataPointTo(point);
      return {data_point_index_temp, nearest_graph_line};
    }

    const auto [pixel_point, data_point, data_point_index_temp] =
        nearest_graph_line->findClosestPixelPointTo(point);
    closest_point = pixel_point;
    closest_data_point = data_point;
    data_point_index = data_point_index_temp;
  }

  return {data_point_index, nearest_graph_line};
}

void Plot::resetLookAndFeelChildrens(juce::LookAndFeel* lookandfeel) {
  for (auto* child : getChildren()) {
    child->setLookAndFeel(lookandfeel);
  }

  m_trace->setLookAndFeel(lookandfeel);
}

Plot::~Plot() { setLookAndFeel(nullptr); }

enum RadioButtonIds { TraceZoomButtons = 1001 };

Plot::Plot(const Scaling x_scaling, const Scaling y_scaling)
    : m_x_scaling(ObserverId::XScaling, x_scaling),
      m_y_scaling(ObserverId::YScaling, y_scaling),
      m_x_lim(ObserverId::XLim),
      m_y_lim(ObserverId::YLim),
      m_graph_bounds(ObserverId::GraphBounds),
      m_common_graph_params(m_graph_bounds, m_x_lim, m_y_lim, m_x_scaling,
                            m_y_scaling, m_downsampling_type),
      m_graph_lines(std::make_unique<GraphLineList>()),
      m_plot_label(std::make_unique<PlotLabel>()),
      m_frame(std::make_unique<Frame>()),
      m_legend(std::make_unique<Legend>()),
      m_selected_area(std::make_unique<GraphArea>()),
      m_grid(std::make_unique<Grid>()),
      m_trace(std::make_unique<Trace>(m_common_graph_params)) {
  m_graph_bounds.addObserver(*m_grid);
  m_x_scaling.addObserver(*m_grid, *m_selected_area);
  m_y_scaling.addObserver(*m_grid, *m_selected_area);
  m_x_lim.addObserver(*m_grid, *m_selected_area);
  m_y_lim.addObserver(*m_grid, *m_selected_area);

  setLookAndFeel(getDefaultLookAndFeel());

  addAndMakeVisible(m_grid.get());
  addChildComponent(m_legend.get());
  addAndMakeVisible(m_selected_area.get());
  addAndMakeVisible(m_plot_label.get());
  addAndMakeVisible(m_frame.get());

  m_legend->setAlwaysOnTop(true);
  m_selected_area->toBehind(m_legend.get());
  m_grid->toBack();

  m_grid->onGridLabelLengthChanged = [this](cmp::Grid* grid) {
    this->resizeChildrens();
  };

  m_legend->onNumberOfDescriptionsChanged = [this](const auto& desc) {
    if (auto lnf = getPlotLookAndFeel()) {
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

  this->setWantsKeyboardFocus(true);
}

PlotLookAndFeel* Plot::getPlotLookAndFeel() {
  auto* lnf = &getLookAndFeel();
  return dynamic_cast<PlotLookAndFeel*>(lnf);
}

void Plot::updateXLim(const Lim_f& new_x_lim) {
  if (new_x_lim.min > new_x_lim.max) UNLIKELY
  throw std::invalid_argument("Min value must be lower than max value.");

  UNLIKELY if ((std::abs(new_x_lim.max - new_x_lim.min) <
                std::numeric_limits<float>::epsilon())) {
    m_x_lim = {new_x_lim.min - 1, new_x_lim.max + 1};
  }

  UNLIKELY if (m_x_scaling == Scaling::logarithmic &&
               new_x_lim.isMinOrMaxZero()) {
    throw std::invalid_argument(
        "The min/max x-value is zero or 'xLim' has been called with a zero "
        "value. 10log(0) = -inf");
  }

  if (new_x_lim && new_x_lim != m_x_lim) {
    m_x_lim = new_x_lim;

    if (m_y_lim.getValue() && !m_x_autoscale) {
      updateGridGraphLinesAndTrace();
    }
  }
}

void Plot::updateYLim(const Lim_f& new_y_lim) {
  if (new_y_lim.min > new_y_lim.max) UNLIKELY
  throw std::invalid_argument("Min value must be lower than max value.");

  UNLIKELY if (std::abs(new_y_lim.max - new_y_lim.min) <
               std::numeric_limits<float>::epsilon()) {
    m_y_lim = {new_y_lim.min - 1, new_y_lim.max + 1};
  }

  UNLIKELY if (m_y_scaling == Scaling::logarithmic &&
               new_y_lim.isMinOrMaxZero()) {
    throw std::invalid_argument(
        "The min/max y-value is zero or 'yLim' has been called with a zero "
        "value. 10log(0) = -inf");
  }

  if (new_y_lim && m_y_lim != new_y_lim) {
    m_y_lim = new_y_lim;

    if (m_x_lim.getValue() && !m_y_autoscale) {
      updateGridGraphLinesAndTrace();
    }
  }
}

void Plot::updateGraphLines() {
  m_graph_lines->setLimitsForVerticalOrHorizontalLines<GraphLineType::vertical, float>(m_y_lim);
  m_graph_lines->setLimitsForVerticalOrHorizontalLines<GraphLineType::horizontal, float>(m_x_lim);

  for (const auto& graph_line : *m_graph_lines) {
    graph_line->updateXIndicesAndPixelPoints();
    graph_line->updateYIndicesAndPixelPoints();
  }
}

void Plot::updateGridGraphLinesAndTrace() {
  if (!m_graph_bounds->isEmpty()) {
    m_grid->update();
    m_trace->updateTracePointsBounds();
    updateGraphLines();
  }

  addSelectableTracePointsForGraphData();
}

void Plot::updateGridAndTracepointsAndGraphLines() {
  if (!m_graph_bounds->isEmpty()) {
    m_grid->update();
    m_trace->updateTracePointsBounds();

    for (const auto& graph_line : *m_graph_lines) {
      graph_line->updateXYPixelPoints();
    }
  }
}

void cmp::Plot::updateTracePointsAndLegends() {
  m_trace->updateTracePointsBounds();

  if (m_legend->isVisible()) {
    m_legend->updateLegends(*m_graph_lines);
  }
}

template <class ValueType>
static auto getLimOffset(const ValueType min, const ValueType max,
                         const cmp::Scaling scaling) {
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
  const auto [min, max] = findMinMaxValuesInGraphLines(*m_graph_lines, true);

  m_x_lim_start = getLimOffset<decltype(min)>(min, max, m_x_scaling);
  m_x_lim_start = createLimsIfTheSame(m_x_lim_start);

  updateXLim(m_x_lim_start);
}

void Plot::setAutoYScale() {
  const auto [min, max] = findMinMaxValuesInGraphLines(*m_graph_lines, false);

  m_y_lim_start = getLimOffset<decltype(min)>(min, max, m_y_scaling);
  m_y_lim_start = createLimsIfTheSame(m_y_lim_start);

  updateYLim(m_y_lim_start);
}

void Plot::xLim(const float min, const float max) {
  updateXLim({min, max});
  m_x_lim_start = {min, max};
  m_x_autoscale = false;
}

void Plot::yLim(const float min, const float max) {
  updateYLim({min, max});
  m_y_lim_start = {min, max};
  m_y_autoscale = false;
}

template <typename ValueType>
static std::pair<std::vector<std::vector<ValueType>>,
                 std::vector<std::vector<ValueType>>>
prepareDataForVerticalOrHorizontalLines(
    const std::vector<ValueType>& coordinates, Lim<ValueType> limits) {
  if (coordinates.empty()) return {{}, {}};

  std::vector<std::vector<ValueType>> lines_start_end(coordinates.size(),
                                                      {limits.min, limits.max});
  std::vector<std::vector<ValueType>> line_coordinates(coordinates.size());

  auto line_coordinate_it = line_coordinates.begin();
  for (auto& x : coordinates) {
    *line_coordinate_it++ = {x, x};
  }

  return {lines_start_end, line_coordinates};
}

void Plot::plotHorizontalLines(const std::vector<float>& y_coordinates,
                               const GraphAttributeList& graph_attributes) {
  auto [x_data, y_data] =
      prepareDataForVerticalOrHorizontalLines<float>(y_coordinates, m_x_lim);
  if (x_data.empty() || y_data.empty()) return;

  plotInternal<GraphLineType::horizontal>(y_data, x_data, graph_attributes);
}

void Plot::plotVerticalLines(const std::vector<float>& x_coordinates,
                             const GraphAttributeList& graph_attributes) {
  auto [y_data, x_data] =
      prepareDataForVerticalOrHorizontalLines<float>(x_coordinates, m_y_lim);
  if (y_data.empty() || x_data.empty()) return;

  plotInternal<GraphLineType::vertical>(y_data, x_data, graph_attributes);
}

template <GraphLineType t_graph_line_type>
void Plot::plotInternal(const std::vector<std::vector<float>>& y_data,
                        const std::vector<std::vector<float>>& x_data,
                        const GraphAttributeList& graph_attributes,
                        const bool update_y_data_only) {
  if (update_y_data_only) jassert(!m_graph_lines->empty());
  // Call cmp::Plot::plot first with x-data to set the x-data.

  updateGraphLineYData<t_graph_line_type>(y_data, graph_attributes);

  if (update_y_data_only) {
    goto skip_update_x_data_label;
  }

  if (!x_data.empty()) {
    updateGraphLineXData<t_graph_line_type>(x_data);
  } else {
    // Fix this for other types of lines
    const auto gen_x_data = generateXdataRamp(y_data);

    if (!gen_x_data.empty()) {
      updateGraphLineXData<t_graph_line_type>(gen_x_data);
    }
  }

skip_update_x_data_label:
  updateGridGraphLinesAndTrace();
}

std::vector<std::vector<float>> Plot::generateXdataRamp(
    const std::vector<std::vector<float>>& y_data) {
  auto generateRamp = [&] {
    std::vector<std::vector<float>> x_data(y_data.size());
    auto x_graph_it = std::begin(x_data);
    for (const auto& y_graph : y_data) {
      (*x_graph_it).resize(y_graph.size());
      std::iota(x_graph_it->begin(), x_graph_it->end(), 1.0f);
      ++x_graph_it;
    }
    return x_data;
  };

  return generateRamp();
}

void Plot::plot(const std::vector<std::vector<float>>& y_data,
                const std::vector<std::vector<float>>& x_data,
                const GraphAttributeList& graph_attributes) {
  plotInternal<GraphLineType::normal>(y_data, x_data, graph_attributes);

  updateTracePointsAndLegends();

  repaint();
}

void Plot::plotUpdateYOnly(const std::vector<std::vector<float>>& y_data) {
  plotInternal<GraphLineType::normal>(y_data, {}, {}, true);

  updateTracePointsAndLegends();

  repaint(m_graph_bounds);
}

void Plot::fillBetween(
    const std::vector<GraphSpreadIndex>& graph_spread_indices,
    const std::vector<juce::Colour>& fill_area_colours) {
  m_graph_spread_list.resize(graph_spread_indices.size());
  auto graph_spread_it = m_graph_spread_list.begin();

  for (const auto& spread_index : graph_spread_indices) {
    if (std::max(spread_index.first_graph, spread_index.second_graph) >=
        m_graph_lines->size<GraphLineType::any>()) {
      throw std::range_error("Spread index out of range.");
    }
    const auto first_graph = (*m_graph_lines)[spread_index.first_graph].get();
    const auto second_graph = (*m_graph_lines)[spread_index.second_graph].get();

    auto it_colour = fill_area_colours.begin();

    const auto colour = it_colour != fill_area_colours.end()
                            ? *it_colour++
                            : first_graph->getColour();

    auto& graph_spread = *graph_spread_it++;
    if (!graph_spread) {
      graph_spread =
          std::make_unique<GraphSpread>(first_graph, second_graph, colour);
      graph_spread->setBounds(m_graph_bounds);
      graph_spread->setLookAndFeel(getPlotLookAndFeel());
      addAndMakeVisible(graph_spread.get());
      graph_spread->toBehind(m_selected_area.get());
    } else {
      graph_spread->m_lower_bound = first_graph;
      graph_spread->m_upper_bound = second_graph;
      graph_spread->m_spread_colour = colour;
      graph_spread->setBounds(m_graph_bounds);
    }
  }
}

void cmp::Plot::setDownsamplingType(const DownsamplingType downsampling_type) {
  this->setDownsamplingTypeInternal(downsampling_type);
}

void cmp::Plot::setDownsamplingTypeInternal(
    const DownsamplingType downsampling_type) {
  if (downsampling_type > DownsamplingType::no_downsampling &&
      m_pixel_point_move_type > PixelPointMoveType::none) {
    jassertfalse;  // You can't change the downsampling type if you configured
                   // it possible to move the pixel points. Call
                   // 'setPixelPointMoveType()' with a value of
                   // 'PixelPointMoveType::none' first.
    m_pixel_point_move_type = PixelPointMoveType::none;
  } else {
    m_downsampling_type = downsampling_type;
  }
  updateGridGraphLinesAndTrace();
}

template <class ValueType>
static auto jassetLogLimBelowZero(const cmp::Scaling scaling,
                                  const Lim<ValueType> lim) {
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
    jassetLogLimBelowZero(x_scaling, m_x_lim_start);
    jassetLogLimBelowZero(y_scaling, m_y_lim_start);

    m_x_scaling = x_scaling;
    m_y_scaling = y_scaling;

    updateGridGraphLinesAndTrace();
  }
}

void Plot::setXLabel(const std::string& x_label) {
  m_plot_label->setXLabel(x_label);

  resizeChildrens();
}

void Plot::setXTickLabels(const std::vector<std::string>& x_labels) {
  m_grid->setXLabels(x_labels);

  if (!m_graph_bounds->isEmpty()) m_grid->update();
}

void Plot::setYTickLabels(const std::vector<std::string>& y_labels) {
  m_grid->setYLabels(y_labels);

  if (!m_graph_bounds->isEmpty()) m_grid->update();
}

void Plot::setXTicks(const std::vector<float>& x_ticks) {
  m_grid->setXTicks(x_ticks);

  if (!m_graph_bounds->isEmpty()) m_grid->update();
}

void Plot::setYTicks(const std::vector<float>& y_ticks) {
  m_grid->setYTicks(y_ticks);

  if (!m_graph_bounds->isEmpty()) m_grid->update();
}

void Plot::setYLabel(const std::string& y_label) {
  m_plot_label->setYLabel(y_label);

  resizeChildrens();
}

void Plot::setTitle(const std::string& title) {
  m_plot_label->setTitle(title);

  resizeChildrens();
}

void Plot::setTracePoint(const juce::Point<float>& trace_point_coordinate) {
  this->setTracePointInternal(trace_point_coordinate, true);
}

// Set trace point internal
void Plot::setTracePointInternal(
    const juce::Point<float>& trace_point_coordinate,
    bool is_point_data_point) {
  const auto [data_point_index, nearest_graph_line] =
      is_point_data_point ? findNearestPoint<true>(trace_point_coordinate)
                          : findNearestPoint<false>(trace_point_coordinate);

  m_trace->addOrRemoveTracePoint(nearest_graph_line, data_point_index);
  m_trace->updateTracePointsBounds();
  m_trace->addAndMakeVisibleTo(this);
}

void Plot::setGridType(const GridType grid_type) {
  m_grid->setGridType(grid_type);
}

void Plot::clearTracePoints() noexcept { m_trace->clear(); }

void Plot::resizeChildrens() {
  if (auto lnf = getPlotLookAndFeel()) {
    const auto plot_bound = lnf->getPlotBounds(getBounds());
    const auto graph_bound = lnf->getGraphBounds(getBounds(), this);

    if (!graph_bound.isEmpty() && m_graph_bounds != graph_bound) {
      m_graph_bounds = graph_bound;

      if (m_grid) {
        m_grid->setBounds(plot_bound);
      }

      if (m_plot_label) m_plot_label->setBounds(plot_bound);

      constexpr auto margin_for_1px_outside = 1;
      const juce::Rectangle<int> frame_bound = {
          graph_bound.getX(), graph_bound.getY(),
          graph_bound.getWidth() + margin_for_1px_outside,
          graph_bound.getHeight() + margin_for_1px_outside};

      if (m_frame) m_frame->setBounds(frame_bound);
      if (m_selected_area) m_selected_area->setBounds(graph_bound);

      for (const auto& graph_line : *m_graph_lines) {
        graph_line->setBounds(graph_bound);
      }

      for (const auto& spread : m_graph_spread_list) {
        spread->setBounds(graph_bound);
      }

      if (m_legend) {
        auto legend_bounds = m_legend->getBounds();
        const auto legend_postion =
            lnf->getLegendPosition(graph_bound, legend_bounds);
        legend_bounds.setPosition(legend_postion);

        m_legend->setBounds(legend_bounds);
      }

      updateGridGraphLinesAndTrace();
    }
  }
}

void Plot::resized() { resizeChildrens(); }

void Plot::paint(juce::Graphics& g) {
  if (getPlotLookAndFeel()) {
    auto lnf = getPlotLookAndFeel();

    lnf->drawBackground(g, m_common_graph_params.graph_bounds);
  }
}

void Plot::parentHierarchyChanged() {
  auto* parentComponent = getParentComponent();
  if (parentComponent) {
    parentComponent->addMouseListener(this, true);
  }
  lookAndFeelChanged();
}

PlotLookAndFeel* Plot::getDefaultLookAndFeel() {
  if (!m_lookandfeel_default)
    m_lookandfeel_default = std::make_unique<PlotLookAndFeel>();
  return m_lookandfeel_default.get();
}

void Plot::lookAndFeelChanged() {
  if (auto* lnf = dynamic_cast<LookAndFeelMethods*>(&getLookAndFeel())) {
    resetLookAndFeelChildrens(lnf);
  } else {
    resetLookAndFeelChildrens(nullptr);
  }
}

template <GraphLineType t_graph_line_type>
void Plot::addGraphLineInternal(std::unique_ptr<GraphLine>& graph_line,
                                const size_t graph_line_index) {
  auto lnf = getPlotLookAndFeel();
  graph_line = std::make_unique<GraphLine>(m_common_graph_params);
  const auto colour_id = lnf->getColourFromGraphID(graph_line_index);
  const auto graph_colour = lnf->findAndGetColourFromId(colour_id);

  graph_line->setColour(graph_colour);
  graph_line->setLookAndFeel(lnf);
  graph_line->setBounds(m_graph_bounds);
  graph_line->setType(t_graph_line_type);

  addAndMakeVisible(graph_line.get());
  graph_line->toBehind(m_selected_area.get());
}

template <GraphLineType t_graph_line_type>
void Plot::updateGraphLineYData(
    const std::vector<std::vector<float>>& y_data,
    const GraphAttributeList& graph_attribute_list) {
  if (y_data.empty()) return;

  UNLIKELY if (y_data.size() != m_graph_lines->size<t_graph_line_type>()) {
    m_graph_lines->resize<t_graph_line_type>(y_data.size());
    std::size_t graph_line_index = 0u;
    for (auto& graph_line : *m_graph_lines) {
      if (graph_line == nullptr) {
        addGraphLineInternal<t_graph_line_type>(graph_line, graph_line_index);
      }
      graph_line_index++;
    }
  }

  auto y_data_it = y_data.begin();
  for (const auto& graph_line : *m_graph_lines) {
    if (graph_line->getType() == t_graph_line_type) {
      if (y_data_it == y_data.end())
        throw std::range_error(
            "Y_data out of range, internal error, please create an issue "
            "on "
            "Github with a test case that triggers this error.");
      graph_line->setYValues(*y_data_it++);
    }
  }

  UNLIKELY if (m_y_autoscale && !is_panning_or_zoomed) { setAutoYScale(); }

  if (!graph_attribute_list.empty()) {
    auto it_gal = graph_attribute_list.begin();
    for (const auto& graph_line : *m_graph_lines) {
      if (it_gal != graph_attribute_list.end() &&
          graph_line->getType() == t_graph_line_type) {
        graph_line->setGraphAttribute(*it_gal++);
      }
    }
  }
}

template void Plot::updateGraphLineYData<GraphLineType::normal>(
    const std::vector<std::vector<float>>& y_data,
    const GraphAttributeList& graph_attribute_list);

template <GraphLineType t_graph_line_type>
void Plot::updateGraphLineXData(const std::vector<std::vector<float>>& x_data) {
  // There is a bug in the code if this assert happens.
  jassert(x_data.size() == m_graph_lines->size<t_graph_line_type>());

  auto x_data_it = x_data.begin();
  for (const auto& graph : *m_graph_lines) {
    if (graph->getType() == t_graph_line_type) {
      graph->setXValues(*x_data_it++);
    }
  }

  if (m_x_autoscale && !is_panning_or_zoomed) {
    setAutoXScale();
  }
}

void Plot::setLegend(const StringVector& graph_descriptions) {
  if (m_legend) {
    m_legend->setVisible(true);
    m_legend->setLegend(graph_descriptions);
    m_legend->updateLegends(*m_graph_lines);
  }
}

void Plot::addOrRemoveTracePoint(const juce::MouseEvent& event) {
  const auto component_pos = event.eventComponent->getBounds().getPosition();

  const auto mouse_pos = getMousePositionRelativeToGraphArea(event);

  const auto [data_point_index, nearest_graph_line] =
      findNearestPoint<false>(mouse_pos, nullptr);

  m_trace->addOrRemoveTracePoint(nearest_graph_line, data_point_index,
                                 TracePointVisibilityType::visible);
  m_trace->updateTracePointsBounds();
  m_trace->addAndMakeVisibleTo(this);
}

void Plot::mouseHandler(const juce::MouseEvent& event,
                        const UserInputAction user_input) {
  switch (user_input) {
    case UserInputAction::create_tracepoint: {
      addOrRemoveTracePoint(event);
      break;
    }
    case UserInputAction::move_tracepoint_to_closest_point: {
      moveTracepoint(event);
      break;
    }
    case UserInputAction::move_tracepoint_label: {
      moveTracepointLabel(event);
      break;
    }
    case UserInputAction::move_legend: {
      moveLegend(event);
      break;
    }
    case UserInputAction::select_tracepoint: {
      selectTracePoint(event);
      break;
    }
    case UserInputAction::deselect_tracepoint: {
      deselectTracePoint(event);
      break;
    }
    case UserInputAction::select_tracepoints_within_selected_area: {
      selectedTracePointsWithinSelectedArea();
      break;
    }
    case UserInputAction::select_area_start: {
      setStartPosSelectedRegion(event.getPosition());
      break;
    }
    case UserInputAction::select_area_draw: {
      drawSelectedRegion(event.getPosition());
      break;
    }
    case UserInputAction::zoom_selected_area: {
      zoomOnSelectedRegion();
      break;
    }
    case UserInputAction::zoom_in: {
      break;
    }
    case UserInputAction::zoom_out: {
      break;
    }
    case UserInputAction::zoom_reset: {
      resetZoom();
      break;
    }
    case UserInputAction::create_movable_pixel_point: {
      break;
    }
    case UserInputAction::move_selected_trace_points: {
      moveSelectedTracePoints(event);
      break;
    }
    case UserInputAction::panning: {
      panning(event);
      break;
    }
    case UserInputAction::remove_movable_pixel_point: {
      break;
    }
    default: {
      break;
    }
  }
}

void Plot::selectTracePoint(const juce::MouseEvent& event) {
  m_trace->selectTracePoint(event.eventComponent, true);
}

void Plot::deselectTracePoint(const juce::MouseEvent& event) {
  for (auto& trace_point : m_trace->getTraceLabelPoints()) {
    m_trace->selectTracePoint(trace_point.trace_point.get(), false);
  }
}

void Plot::moveSelectedTracePoints(const juce::MouseEvent& event) {
  const auto mouse_pos = getMousePositionRelativeToGraphArea(event);

  auto d_data_position =
      getDataPointFromPixelCoordinate(mouse_pos, m_common_graph_params) -
      getDataPointFromPixelCoordinate(m_prev_mouse_position,
                                      m_common_graph_params);

  switch (m_pixel_point_move_type) {
    case PixelPointMoveType::horizontal: {
      d_data_position.setY(0.f);
      break;
    }
    case PixelPointMoveType::vertical: {
      d_data_position.setX(0.f);
      break;
    }
    case PixelPointMoveType::horizontal_vertical: {
      break;
    }
    default: {
      break;
    }
  }

  std::map<GraphLine*, std::vector<size_t>> graph_line_data_point_map;
  for (const auto& trace_label_point : m_trace->getTraceLabelPoints()) {
    if (!trace_label_point.isSelected()) continue;
    const auto graph_line =
        trace_label_point.trace_point->associated_graph_line;
    const auto data_point_index =
        trace_label_point.trace_point->data_point_index;

    for (const auto& graph : *m_graph_lines) {
      if (graph.get() == graph_line) {
        graph->movePixelPoint(d_data_position, data_point_index);
        graph_line_data_point_map[graph.get()].push_back(data_point_index);
        break;
      }
    }
  }

  for (auto& [graph_line, indices_to_update] : graph_line_data_point_map) {
    graph_line->updateXIndicesAndPixelPoints(indices_to_update);
    graph_line->updateYIndicesAndPixelPoints(indices_to_update);
  }

  m_trace->updateTracePointsBounds();
  m_prev_mouse_position = mouse_pos;

  repaint();

  if (m_graph_lines_changed_callback) {
    m_graph_lines_changed_callback(createGraphLineDataViewList(*m_graph_lines));
  }
}

void Plot::resetZoom() {
  is_panning_or_zoomed = false;
  updateXLim(m_x_lim_start);
  updateYLim(m_y_lim_start);
  updateGridGraphLinesAndTrace();
  repaint();
}

void Plot::setStartPosSelectedRegion(const juce::Point<int>& start_position) {
  for (auto& trace_point : m_trace->getTraceLabelPoints()) {
    m_trace->selectTracePoint(trace_point.trace_point.get(), false);
  }

  m_selected_area->setStartPosition(start_position);
}

void Plot::drawSelectedRegion(const juce::Point<int>& end_position) {
  m_selected_area->setEndPosition(end_position);
  m_selected_area->repaint();
}

void Plot::zoomOnSelectedRegion() {
  is_panning_or_zoomed = true;
  const auto data_bound = m_selected_area->getDataBound<float>();

  updateXLim({data_bound.getX(), data_bound.getX() + data_bound.getWidth()});
  updateYLim({data_bound.getY(), data_bound.getY() + data_bound.getHeight()});
  updateGridGraphLinesAndTrace();

  m_selected_area->reset();
  repaint();
}

void Plot::selectedTracePointsWithinSelectedArea() {
  const auto selected_area = m_selected_area->getSelectedAreaBound<int>();
  constexpr auto margin = juce::Point<int>(2, 5);

  for (auto& trace_point : m_trace->getTraceLabelPoints()) {
    auto trace_point_pos = trace_point.trace_point->getBounds().getPosition() -
                           m_graph_bounds->getPosition() + margin;

    if (selected_area.contains(trace_point_pos)) {
      m_trace->selectTracePoint(trace_point.trace_point.get(), true);
    }
  }

  m_trace->updateTracePointsBounds();
  m_trace->addAndMakeVisibleTo(this);

  m_selected_area->reset();
  repaint();
}

void Plot::moveTracepoint(const juce::MouseEvent& event) {
  auto bounds = event.eventComponent->getBounds();

  const auto mouse_pos =
      bounds.getPosition() - m_graph_bounds->getPosition() +
      event.getEventRelativeTo(event.eventComponent).getPosition();

  const auto* associated_graph_line =
      m_trace->getAssociatedGraphLine(event.eventComponent);

  const auto [data_point_index, nearest_graph_line] =
      findNearestPoint(mouse_pos.toFloat(), associated_graph_line);

  m_trace->setDataPointFor(event.eventComponent, data_point_index,
                           nearest_graph_line);
}

void Plot::moveTracepointLabel(const juce::MouseEvent& event) {
  auto bounds = event.eventComponent->getBounds();

  const auto mouse_pos =
      bounds.getPosition() +
      event.getEventRelativeTo(event.eventComponent).getPosition();

  if (m_trace->setCornerPositionForLabelAssociatedWith(event.eventComponent,
                                                       mouse_pos)) {
    m_trace->updateSingleTracePointBoundsFrom(event.eventComponent);
  }
}

void Plot::moveLegend(const juce::MouseEvent& event) {
  m_comp_dragger.dragComponent(event.eventComponent, event, nullptr);
}

void Plot::mouseDown(const juce::MouseEvent& event) {
  if (isVisible()) {
    m_prev_mouse_position = getMousePositionRelativeToGraphArea(event);

    const auto lnf = getPlotLookAndFeel();
    if (m_selected_area.get() == event.eventComponent) {
      if (event.mods.isRightButtonDown()) {
        mouseHandler(
            event, lnf->getUserInputAction(UserInput::right | UserInput::drag |
                                           UserInput::graph_area));
      }
    }

    if (m_trace->isComponentTracePoint(event.eventComponent)) {
      if (!event.mods.isRightButtonDown()) {
        mouseHandler(
            event, lnf->getUserInputAction(UserInput::left | UserInput::start |
                                           UserInput::tracepoint));
      }
    }

    if (m_mouse_drag_state == MouseDragState::none) {
      m_mouse_drag_state = MouseDragState::start;
    }

    if (event.getNumberOfClicks() > 1) {
      mouseHandler(event, lnf->getUserInputAction(UserInput::left |
                                                  UserInput::double_click |
                                                  UserInput::graph_area));
    }
  }
}

void Plot::mouseDrag(const juce::MouseEvent& event) {
  if (isVisible()) {
    const auto lnf = getPlotLookAndFeel();
    if (m_legend.get() == event.eventComponent) {
      mouseHandler(event,
                   lnf->getUserInputAction(UserInput::left | UserInput::drag |
                                           UserInput::legend));
    } else if (m_selected_area.get() == event.eventComponent &&
               event.mouseWasDraggedSinceMouseDown() &&
               event.getNumberOfClicks() == 1) {
      if (m_modifiers && m_modifiers->isCommandDown()) {
        mouseHandler(event, lnf->getUserInputAction(
                                UserInput::left | UserInput::drag |
                                UserInput::ctrl | UserInput::graph_area));
      } else if (m_mouse_drag_state == MouseDragState::start) {
        mouseHandler(event, lnf->getUserInputAction(
                                UserInput::left | UserInput::drag |
                                UserInput::start | UserInput::graph_area));
        m_mouse_drag_state = MouseDragState::drag;
      } else {
        mouseHandler(event,
                     lnf->getUserInputAction(UserInput::left | UserInput::drag |
                                             UserInput::graph_area));
        m_mouse_drag_state = MouseDragState::drag;
      }
    } else if (m_trace->isComponentTracePoint(event.eventComponent) &&
               event.getNumberOfClicks() == 1) {
      mouseHandler(event,
                   lnf->getUserInputAction(UserInput::left | UserInput::drag |
                                           UserInput::tracepoint));
    } else if (m_trace->isComponentTraceLabel(event.eventComponent)) {
      mouseHandler(event,
                   lnf->getUserInputAction(UserInput::left | UserInput::drag |
                                           UserInput::trace_label));
    }
  }
}

void Plot::mouseUp(const juce::MouseEvent& event) {
  if (isVisible()) {
    const auto lnf = getPlotLookAndFeel();
    if (m_selected_area.get() == event.eventComponent &&
        m_mouse_drag_state == MouseDragState::drag) {
      if (!event.mods.isRightButtonDown()) {
        mouseHandler(event, lnf->getUserInputAction(
                                UserInput::left | UserInput::drag |
                                UserInput::end | UserInput::graph_area));
      }

      m_mouse_drag_state = MouseDragState::none;
    }

    if (m_trace->isComponentTracePoint(event.eventComponent)) {
      if (!event.mods.isRightButtonDown()) {
        mouseHandler(event,
                     lnf->getUserInputAction(UserInput::left | UserInput::end |
                                             UserInput::tracepoint));
      }
    }
  }
}

void Plot::modifierKeysChanged(const juce::ModifierKeys& modifiers) {
  m_modifiers = &modifiers;
}

void Plot::setMovePointsType(const PixelPointMoveType move_points_type) {
  m_pixel_point_move_type = move_points_type;
  updateTracepointsForGraphData();
}

void Plot::addSelectableTracePointsForGraphData() {
  if (m_pixel_point_move_type == PixelPointMoveType::none) return;
  m_trace->clear();

  for (const auto& graph_line : *m_graph_lines) {
    const auto& y_values = graph_line->getYData();
    size_t data_point_index = 0;

    for (; data_point_index < y_values.size(); data_point_index++) {
      m_trace->addTracePoint(
          graph_line.get(), data_point_index,
          TracePointVisibilityType::point_visible_when_selected);
    }
  }

  m_trace->updateTracePointsBounds();
  m_trace->addAndMakeVisibleTo(this);
}

void Plot::updateTracepointsForGraphData() {
  switch (m_pixel_point_move_type) {
    case PixelPointMoveType::none:
      return;
      break;
    case PixelPointMoveType::horizontal:
      this->setDownsamplingTypeInternal(DownsamplingType::no_downsampling);
      break;
    case PixelPointMoveType::vertical:
      this->setDownsamplingTypeInternal(DownsamplingType::no_downsampling);
      break;
    case PixelPointMoveType::horizontal_vertical:
      this->setDownsamplingTypeInternal(DownsamplingType::no_downsampling);
      break;
  }
}

juce::Point<float> Plot::getMousePositionRelativeToGraphArea(
    const juce::MouseEvent& event) const {
  if (event.eventComponent != m_selected_area.get()) {
    const auto component_pos = event.eventComponent->getBounds().getPosition();

    const auto mouse_pos =
        (event.getPosition() + component_pos - m_graph_bounds->getPosition())
            .toFloat();

    return mouse_pos;
  }

  return event.getPosition().toFloat();
}

void Plot::setGraphLineDataChangedCallback(
    GraphLinesChangedCallback graph_lines_changed_callback) {
  m_graph_lines_changed_callback = graph_lines_changed_callback;
}

void Plot::panning(const juce::MouseEvent& event) {
  const auto mouse_pos = getMousePositionRelativeToGraphArea(event);
  const auto d_mouse_pos = mouse_pos - m_prev_mouse_position;
  const auto new_x_lim_min_pixel = m_graph_bounds->getX() - d_mouse_pos.getX();
  const auto new_x_lim_max_pixel = m_graph_bounds->getRight() - d_mouse_pos.getX();
  const auto new_x_lim_data_min = getXDataFromXPixelCoordinate(new_x_lim_min_pixel, m_graph_bounds->toFloat(), m_x_lim, m_x_scaling);
  const auto new_x_lim_data_max = getXDataFromXPixelCoordinate(new_x_lim_max_pixel, m_graph_bounds->toFloat(), m_x_lim, m_x_scaling);
  const auto new_x_lim_data = Lim_f(new_x_lim_data_min, new_x_lim_data_max);

  const auto new_y_lim_min_pos = m_graph_bounds->getBottom() - d_mouse_pos.getY();
  const auto new_y_lim_max_pos = m_graph_bounds->getY() - d_mouse_pos.getY();
  const auto new_y_lim_data_min = getYDataFromYPixelCoordinate(new_y_lim_min_pos, m_graph_bounds->toFloat(), m_y_lim, m_y_scaling);
  const auto new_y_lim_data_max = getYDataFromYPixelCoordinate(new_y_lim_max_pos, m_graph_bounds->toFloat(), m_y_lim, m_y_scaling);
  const auto new_y_lim_data = Lim_f(new_y_lim_data_min, new_y_lim_data_max);
 
  m_prev_mouse_position = mouse_pos;
  is_panning_or_zoomed = true;

  this->updateXLim(new_x_lim_data);
  this->updateYLim(new_y_lim_data);

  updateGridAndTracepointsAndGraphLines();
  repaint();
}

}  // namespace cmp
