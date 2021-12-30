#include "spl_grid.h"

#include <stdexcept>
#include <tuple>

#include "scp_internal_datamodels.h"
#include "spl_graph_line.h"
#include "spl_plot.h"

namespace scp {

/*============================================================================*/

void Grid::createLabels() {
  if (m_lookandfeel) {
    auto lnf = static_cast<Plot::LookAndFeelMethods *>(m_lookandfeel);
    const auto graph_bounds = juce::Rectangle<int>(m_config_params.grid_area);
    lnf->updateGridLabels(graph_bounds, m_grid_lines, m_custom_x_labels,
                          m_custom_y_labels, m_x_axis_labels, m_y_axis_labels);
  }
}

void Grid::updateGridInternal() {
  if (!m_config_params.x_lim || !m_config_params.y_lim) {
    // Both x_lim and y_lim must be set.
    jassertfalse;
    return;
  }

  if (getBounds().getWidth() <= 0 && getBounds().getHeight() <= 0) {
    // width and height must be larger than zero.
    jassertfalse;
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

  if (onNumGridsChange && m_num_last_x_labels != m_x_axis_labels.size() &&
      m_last_num_y_labels != m_y_axis_labels.size()) {
    m_num_last_x_labels = m_x_axis_labels.size();
    m_last_num_y_labels = m_y_axis_labels.size();
    onNumGridsChange(this);
  }
}

void Grid::addGridLines(const std::vector<float> &ticks,
                        const GridLine::Direction direction) {
  if (m_lookandfeel) {
    auto lnf = static_cast<Plot::LookAndFeelMethods *>(m_lookandfeel);

    const auto graph_bounds =
        juce::Rectangle<int>(m_config_params.grid_area).toFloat();

    const auto getScaleOffset = [&]() {
      if (direction == GridLine::Direction::vertical) {
        return getXScaleAndOffset(graph_bounds.getWidth(),
                                  Lim_f(m_config_params.x_lim),
                                  lnf->getXScaling());
      }
      return getYScaleAndOffset(graph_bounds.getHeight(),
                                Lim_f(m_config_params.y_lim),
                                lnf->getYScaling());
    };

    const auto [scale, offset] = getScaleOffset();

    switch (direction) {
      case GridLine::Direction::vertical:
        for (const auto t : ticks) {
          GridLine grid_line;

          grid_line.position = {
              graph_bounds.getX() +
                  (lnf->getXScaling() == Scaling::linear
                       ? getXGraphValueLinear(t, scale, offset)
                       : getXGraphPointsLogarithmic(t, scale, offset)),
              graph_bounds.getY()};

          grid_line.tick = t;
          grid_line.length = graph_bounds.getHeight();
          grid_line.direction = GridLine::Direction::vertical;

          m_grid_lines.emplace_back(grid_line);
        }
        break;

      case GridLine::Direction::horizontal:
        for (const auto t : ticks) {
          GridLine grid_line;

          grid_line.position = {
              graph_bounds.getX(),
              graph_bounds.getY() +
                  (lnf->getYScaling() == Scaling::linear
                       ? getYGraphValueLinear(t, scale, offset)
                       : getYGraphPointsLogarithmic(t, scale, offset))};

          grid_line.tick = t;
          grid_line.direction = GridLine::Direction::horizontal;
          grid_line.length = float(graph_bounds.getWidth());

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

const std::pair<int, int> Grid::getMaxGridLabelWidth() const noexcept {
  if (m_lookandfeel && !m_x_axis_labels.empty() && !m_y_axis_labels.empty()) {
    auto lnf = static_cast<Plot::LookAndFeelMethods *>(m_lookandfeel);
    const auto font = lnf->getGridLabelFont();

    const auto widest_x_label =
        *std::max_element(m_x_axis_labels.begin(), m_x_axis_labels.end(),
                          [&](const auto &label1, const auto &label2) {
                            return font.getStringWidth(label1.first) <
                                   font.getStringWidth(label2.first);
                          });

    const auto widest_y_label =
        *std::max_element(m_y_axis_labels.begin(), m_y_axis_labels.end(),
                          [&](const auto &label1, const auto &label2) {
                            return font.getStringWidth(label1.first) <
                                   font.getStringWidth(label2.first);
                          });
    const auto max_width_x = font.getStringWidth(widest_x_label.first);
    const auto max_width_y = font.getStringWidth(widest_y_label.first);

    return {max_width_x, max_width_y};
  }
  return {0, 0};
}

void Grid::paint(juce::Graphics &g) {
  if (m_lookandfeel) {
    auto lnf = static_cast<Plot::LookAndFeelMethods *>(m_lookandfeel);
    lnf->drawGridLabels(g, m_x_axis_labels, m_y_axis_labels);

    for (const auto &grid_line : m_grid_lines) {
      lnf->drawGridLine(g, grid_line, m_config_params.grid_on);
    }
  }
}

void Grid::lookAndFeelChanged() {
  if (auto *lnf = dynamic_cast<Plot::LookAndFeelMethods *>(&getLookAndFeel())) {
    m_lookandfeel = lnf;
  } else {
    m_lookandfeel = nullptr;
  }
}

void Grid::setXLabels(const std::vector<std::string> &x_labels) {
  m_custom_x_labels = x_labels;
}

void Grid::updateGrid() { updateGridInternal(); }

void Grid::resized() {}
void Grid::setGridBounds(const juce::Rectangle<int> &grid_area) {
  m_config_params.grid_area = grid_area;
}

void Grid::setYLim(const float min, const float max) {
  m_config_params.y_lim = {min, max};
}

void Grid::setXLim(const float min, const float max) {
  m_config_params.x_lim = {min, max};
}

void Grid::setGridON(const bool grid_on, const bool tiny_grids_on) {
  if (grid_on) m_config_params.grid_on = grid_on;
  if (tiny_grids_on) m_config_params.tiny_grid_on = tiny_grids_on;
}

void Grid::setXTicks(const std::vector<float> &x_ticks) {
  m_custom_x_ticks = x_ticks;
}

void Grid::setYLabels(const std::vector<std::string> &y_labels) {
  m_custom_y_labels = y_labels;
}

void Grid::setYTicks(const std::vector<float> &y_ticks) {
  m_custom_y_ticks = y_ticks;
}

void Grid::createAutoGridTicks(std::vector<float> &x_ticks,
                               std::vector<float> &y_ticks) {
  if (m_lookandfeel) {
    if (auto *lnf =
            static_cast<scp::Plot::LookAndFeelMethods *>(m_lookandfeel)) {
      lnf->updateVerticalGridLineTicksAuto(getBounds(),
                                           m_config_params.tiny_grid_on,
                                           m_config_params.x_lim, x_ticks);
      lnf->updateHorizontalGridLineTicksAuto(getBounds(),
                                             m_config_params.tiny_grid_on,
                                             m_config_params.y_lim, y_ticks);
    }
  }
}
}  // namespace scp