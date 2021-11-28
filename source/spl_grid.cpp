#include "spl_grid.h"

#include <stdexcept>
#include <tuple>

#include "scp_internal_datamodels.h"
#include "spl_graph_line.h"
#include "spl_plot.h"

namespace scp {

/*============================================================================*/

template <Scaling x_scaling_T, Scaling y_scaling_T>
void Grid<x_scaling_T, y_scaling_T>::createLabels() {
  if (m_lookandfeel) {
    auto lnf = static_cast<Plot::LookAndFeelMethods *>(m_lookandfeel);
    lnf->updateGridLabels(getBounds(), m_grid_lines, m_custom_x_labels,
                          m_custom_y_labels, m_x_axis_labels, m_y_axis_labels);
  }
}

template <Scaling x_scaling_T, Scaling y_scaling_T>
void Grid<x_scaling_T, y_scaling_T>::updateGridInternal() {
  if (!m_config_params.x_lim || !m_config_params.y_lim) {
    DBG("Both x_lim and y_lim must be set.");
    return;
  }

  if (getBounds().getWidth() <= 0 && getBounds().getHeight() <= 0) {
    DBG("width and height must be larger than zero.");
    return;
  }

  // Temp, should be removed
  if (m_lookandfeel) {
    auto lnf = static_cast<Plot::LookAndFeelMethods *>(m_lookandfeel);
    m_config_params.grid_area = lnf->getGraphBounds(getBounds());
  } else {
    return;
  }

  std::vector<float> x_auto_ticks, y_auto_ticks;

  if (m_custom_x_ticks.empty() || m_custom_y_ticks.empty())
    createAutoGridTicks(x_auto_ticks, y_auto_ticks);

  const auto &x_ticks =
      m_custom_x_ticks.empty() ? x_auto_ticks : m_custom_x_ticks;
  const auto &y_ticks =
      m_custom_y_ticks.empty() ? y_auto_ticks : m_custom_y_ticks;

  m_grid_lines.clear();
  m_grid_lines.reserve(x_ticks.size() + y_ticks.size());

  addGridLines(x_ticks, GridLine::Direction::vertical);
  addGridLines(y_ticks, GridLine::Direction::horizontal);

  createLabels();
}

template <Scaling x_scaling_T, Scaling y_scaling_T>
void Grid<x_scaling_T, y_scaling_T>::addGridLines(
    const std::vector<float> &ticks, const GridLine::Direction direction) {
  if (m_lookandfeel) {
    auto lnf = static_cast<Plot::LookAndFeelMethods *>(m_lookandfeel);

    const auto graph_bound = lnf->getGraphBounds(getBounds()).toFloat();

    const auto getScaleOffset = [&]() {
      if (direction == GridLine::Direction::vertical) {
        return getXScaleAndOffset(float(graph_bound.getWidth()),
                                  Lim_f(m_config_params.x_lim), x_scaling);
      }
      return getYScaleAndOffset(float(graph_bound.getHeight()),
                                Lim_f(m_config_params.y_lim), y_scaling);
    };

    const auto [scale, offset] = getScaleOffset();

    switch (direction) {
      case GridLine::Direction::vertical:
        for (const auto t : ticks) {
          GridLine grid_line;

          grid_line.position = {
              graph_bound.getX() +
                  (x_scaling == Scaling::linear
                       ? getXGraphPointsLinear(t, scale, offset)
                       : getXGraphPointsLogarithmic(t, scale, offset)),
              graph_bound.getY()};

          grid_line.tick = t;
          grid_line.length = float(graph_bound.getHeight());
          grid_line.direction = GridLine::Direction::vertical;

          m_grid_lines.emplace_back(grid_line);
        }
        break;

      case GridLine::Direction::horizontal:
        for (const auto t : ticks) {
          GridLine grid_line;

          grid_line.position = {
              graph_bound.getX(),
              graph_bound.getY() +
                  (y_scaling == Scaling::linear
                       ? getYGraphPointsLinear(t, scale, offset)
                       : getYGraphPointsLogarithmic(t, scale, offset))};

          grid_line.tick = t;
          grid_line.direction = GridLine::Direction::horizontal;
          grid_line.length = float(graph_bound.getWidth());

          m_grid_lines.emplace_back(grid_line);
        }
        break;

      default:
        // invalid direction.
        jassertfalse;
        break;
    }
  }
}

template <Scaling x_scaling_T, Scaling y_scaling_T>
void Grid<x_scaling_T, y_scaling_T>::paint(juce::Graphics &g) {
  if (m_lookandfeel) {
    auto lnf = static_cast<Plot::LookAndFeelMethods *>(m_lookandfeel);
    lnf->drawGridLabels(g, m_x_axis_labels, m_y_axis_labels);

    for (const auto &grid_line : m_grid_lines) {
      lnf->drawGridLine(g, grid_line, m_config_params.grid_on);
    }
  }
}

template <Scaling x_scaling_T, Scaling y_scaling_T>
void Grid<x_scaling_T, y_scaling_T>::lookAndFeelChanged() {
  if (auto *lnf = dynamic_cast<Plot::LookAndFeelMethods *>(&getLookAndFeel())) {
    m_lookandfeel = lnf;
  } else {
    m_lookandfeel = nullptr;
  }
}

template <Scaling x_scaling_T, Scaling y_scaling_T>
void Grid<x_scaling_T, y_scaling_T>::setXLabels(
    const std::vector<std::string> &x_labels) {
  m_custom_x_labels = x_labels;
}

template <Scaling x_scaling_T, Scaling y_scaling_T>
void Grid<x_scaling_T, y_scaling_T>::updateGrid() {
  updateGridInternal();
}

template <Scaling x_scaling_T, Scaling y_scaling_T>
void Grid<x_scaling_T, y_scaling_T>::resized() {
  updateGridInternal();
}

template <Scaling x_scaling_T, Scaling y_scaling_T>
void Grid<x_scaling_T, y_scaling_T>::setGridBounds(
    const juce::Rectangle<int> &grid_area) {
  m_config_params.grid_area = grid_area;
}

template <Scaling x_scaling_T, Scaling y_scaling_T>
void Grid<x_scaling_T, y_scaling_T>::setYLim(const float min, const float max) {
  m_config_params.y_lim = {min, max};
}

template <Scaling x_scaling_T, Scaling y_scaling_T>
void Grid<x_scaling_T, y_scaling_T>::setXLim(const float min, const float max) {
  m_config_params.x_lim = {min, max};
}

template <Scaling x_scaling_T, Scaling y_scaling_T>
void Grid<x_scaling_T, y_scaling_T>::setGridON(const bool grid_on,
                                               const bool tiny_grids_on) {
  if (grid_on) m_config_params.grid_on = grid_on;
  if (tiny_grids_on) m_config_params.tiny_grid_on = tiny_grids_on;
}

template <Scaling x_scaling_T, Scaling y_scaling_T>
void Grid<x_scaling_T, y_scaling_T>::setXTicks(
    const std::vector<float> &x_ticks) {
  m_custom_x_ticks = x_ticks;
}

template <Scaling x_scaling_T, Scaling y_scaling_T>
void Grid<x_scaling_T, y_scaling_T>::setYLabels(
    const std::vector<std::string> &y_labels) {
  m_custom_y_labels = y_labels;
}

template <Scaling x_scaling_T, Scaling y_scaling_T>
void Grid<x_scaling_T, y_scaling_T>::setYTicks(
    const std::vector<float> &y_ticks) {
  m_custom_y_ticks = y_ticks;
}

template <Scaling x_scaling_T, Scaling y_scaling_T>
void Grid<x_scaling_T, y_scaling_T>::createAutoGridTicks(
    std::vector<float> &x_ticks, std::vector<float> &y_ticks) {
  if (m_lookandfeel) {
    if (auto *lnf =
            static_cast<scp::Plot::LookAndFeelMethods *>(m_lookandfeel)) {
      lnf->updateVerticalGridLineTicksAuto(getBounds(), x_scaling,
                                           m_config_params.tiny_grid_on,
                                           m_config_params.x_lim, x_ticks);
      lnf->updateHorizontalGridLineTicksAuto(getBounds(), y_scaling,
                                             m_config_params.tiny_grid_on,
                                             m_config_params.y_lim, y_ticks);
    }
  }
}

// Explicit template instantiation
template class Grid<Scaling::linear, Scaling::linear>;
template class Grid<Scaling::logarithmic, Scaling::linear>;
template class Grid<Scaling::linear, Scaling::logarithmic>;
template class Grid<Scaling::logarithmic, Scaling::logarithmic>;

}  // namespace scp