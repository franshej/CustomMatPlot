/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "cmp_grid.h"

#include <stdexcept>
#include <tuple>

#include "cmp_graph_line.h"
#include "cmp_plot.h"
#include "cmp_utils.h"

namespace cmp {

/*============================================================================*/

static juce::Rectangle<float> getMarginGridBound(
    const juce::Rectangle<float> &bound) noexcept {
  constexpr auto safe_marging_offset_px = 1.0f;
  const auto x = bound.getX();
  const auto y = bound.getY();
  const auto w = bound.getWidth() + safe_marging_offset_px;
  const auto h = bound.getHeight() + safe_marging_offset_px;

  return {x, y, w, h};
}

void Grid::createLabels() {
  if (m_lookandfeel) {
    auto lnf = static_cast<Plot::LookAndFeelMethods *>(m_lookandfeel);
    lnf->updateGridLabels(*m_common_plot_params, m_grid_lines,
                          m_custom_x_labels, m_custom_y_labels, m_x_axis_labels,
                          m_y_axis_labels);
  }
}

void Grid::updateGridInternal(const bool use_cached_grids) {
  if (getBounds().getWidth() <= 0 && getBounds().getHeight() <= 0) {
    // width and height must be larger than zero.
    jassertfalse;
    return;
  }

  if (!use_cached_grids) {
    m_x_prev_ticks.clear();
    m_y_prev_ticks.clear();
  }

  std::vector<float> x_auto_ticks, y_auto_ticks;

  if (m_custom_x_ticks.empty() || m_custom_y_ticks.empty()) {
    createAutoGridTicks(x_auto_ticks, y_auto_ticks);
  }

  const auto &x_ticks =
      m_custom_x_ticks.empty() ? x_auto_ticks : m_custom_x_ticks;
  const auto &y_ticks =
      m_custom_y_ticks.empty() ? y_auto_ticks : m_custom_y_ticks;

  m_grid_lines.clear();
  m_grid_lines.reserve(x_ticks.size() + y_ticks.size());

  addGridLines(x_ticks, GridLine::Direction::vertical);
  addGridLines(y_ticks, GridLine::Direction::horizontal);
  createLabels();

  if (m_grid_type >= GridType::grid_translucent) {
    addTranslucentGridLines();
  }

  if (onGridLabelLengthChanged && m_lookandfeel) {
    const auto lnf = static_cast<Plot::LookAndFeelMethods *>(m_lookandfeel);
    const auto font = lnf->getGridLabelFont();

    auto longest_label_x_axis = 0;
    for (const auto &label : m_x_axis_labels) {
      const auto current_label_length = font.getStringWidth(label.first);
      if (current_label_length > longest_label_x_axis) {
        longest_label_x_axis = current_label_length;
      }
    }

    auto longest_label_y_axis = 0;
    for (const auto &label : m_y_axis_labels) {
      const auto current_label_length = font.getStringWidth(label.first);
      if (current_label_length > longest_label_y_axis) {
        longest_label_y_axis = current_label_length;
      }
    }

    if (longest_label_x_axis > (m_longest_x_axis_label_length_last_cb_triggerd +
                                lnf->getMargin()) ||
        longest_label_y_axis > (m_longest_y_axis_label_length_last_cb_triggerd +
                                lnf->getMargin()) ||
        float(longest_label_x_axis) <
            std::abs(float(m_longest_x_axis_label_length_last_cb_triggerd -
                           lnf->getMargin())) ||
        float(longest_label_y_axis) <
            std::abs(float(m_longest_y_axis_label_length_last_cb_triggerd -
                           lnf->getMargin()))) {
      m_longest_x_axis_label_length_last_cb_triggerd = longest_label_x_axis;
      m_longest_y_axis_label_length_last_cb_triggerd = longest_label_y_axis;

      onGridLabelLengthChanged(this);
    }
  }
}

void Grid::addGridLines(const std::vector<float> &ticks,
                        const GridLine::Direction direction) {
  if (m_lookandfeel) {
    auto lnf = static_cast<Plot::LookAndFeelMethods *>(m_lookandfeel);

    const auto graph_bounds =
        juce::Rectangle<int>(m_common_plot_params->graph_bounds).toFloat();

    const auto getScaleOffset = [&]() {
      if (direction == GridLine::Direction::vertical) {
        return getXScaleAndOffset(graph_bounds.getWidth(),
                                  m_common_plot_params->x_lim,
                                  m_common_plot_params->x_scaling);
      }
      return getYScaleAndOffset(graph_bounds.getHeight(),
                                m_common_plot_params->y_lim,
                                m_common_plot_params->y_scaling);
    };

    const auto [scale, offset] = getScaleOffset();
    const auto margin_grid_bound = getMarginGridBound(graph_bounds);

    switch (direction) {
      case GridLine::Direction::vertical:
        for (const auto t : ticks) {
          GridLine grid_line;

          grid_line.position = {
              graph_bounds.getX() +
                  (m_common_plot_params->x_scaling == Scaling::linear
                       ? getXGraphValueLinear(t, scale, offset)
                       : getXGraphPointsLogarithmic(t, scale, offset)),
              graph_bounds.getY()};

          if (!margin_grid_bound.contains(grid_line.position)) {
            continue;
          }

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
              std::ceil(graph_bounds.getY() +
                        (m_common_plot_params->y_scaling == Scaling::linear
                             ? getYGraphValueLinear(t, scale, offset)
                             : getYGraphPointsLogarithmic(t, scale, offset)))};

          if (!margin_grid_bound.contains(grid_line.position)) {
            continue;
          }

          grid_line.tick = t;
          grid_line.direction = GridLine::Direction::horizontal;
          grid_line.length = graph_bounds.getWidth();

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

void Grid::addTranslucentGridLines() {
  auto getTranslucentGridLine =
      [](const auto *prev_grid_line, const auto *grid_line,
         const bool is_minor_grid_line_in_front = true) {
        auto translucent_gridline = *prev_grid_line;
        auto sign = is_minor_grid_line_in_front ? 1 : -1;
        translucent_gridline.type = GridLine::Type::translucent;
        if (grid_line->direction == GridLine::Direction::vertical) {
          translucent_gridline.position.x +=
              sign * (grid_line->position.x - prev_grid_line->position.x) / 2;
        } else {
          translucent_gridline.position.y +=
              sign * (grid_line->position.y - prev_grid_line->position.y) / 2;
        }

        return translucent_gridline;
      };

  std::vector<GridLine> translucent_gridlines;
  const GridLine *prev_gridline = nullptr;
  for (auto grid_line = m_grid_lines.begin(); grid_line != m_grid_lines.end();
       ++grid_line) {
    auto addGridWithinBound = [&](const GridLine &grid_line) {
      const auto margin_grid_bound =
          getMarginGridBound(m_common_plot_params->graph_bounds.toFloat());
      if (margin_grid_bound.contains(grid_line.position)) {
        translucent_gridlines.emplace_back(grid_line);
      }
    };

    GridLine translucent_grid_line;
    const auto next_it = std::next(grid_line, 1);
    if (!prev_gridline && next_it != m_grid_lines.end() &&
        next_it->direction == grid_line->direction)
      UNLIKELY {
        translucent_grid_line =
            getTranslucentGridLine(&(*grid_line), &(*next_it), false);
      }
    else if (!prev_gridline) {
      prev_gridline = &(*grid_line);
      continue;
    } else if (next_it == m_grid_lines.end() ||
               (next_it->direction != grid_line->direction) &&
                   prev_gridline->direction == grid_line->direction)
      UNLIKELY {
        translucent_grid_line =
            getTranslucentGridLine(&(*grid_line), prev_gridline, false);
        addGridWithinBound(translucent_grid_line);
        translucent_grid_line =
            getTranslucentGridLine(prev_gridline, &(*grid_line));
      }
    else if (prev_gridline->direction == grid_line->direction) {
      translucent_grid_line =
          getTranslucentGridLine(prev_gridline, &(*grid_line));
    } else if (prev_gridline->direction != grid_line->direction &&
               next_it->direction == grid_line->direction) {
      translucent_grid_line =
          getTranslucentGridLine(&(*grid_line), &(*next_it), false);
    }
    prev_gridline = &(*grid_line);
    addGridWithinBound(translucent_grid_line);
  }

  m_grid_lines.insert(m_grid_lines.end(), translucent_gridlines.begin(),
                      translucent_gridlines.end());
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
    for (const auto &grid_line : m_grid_lines) {
      lnf->drawGridLine(g, grid_line, m_grid_type);
    }
    lnf->drawGridLabels(g, m_x_axis_labels, m_y_axis_labels);
  }
}

void Grid::lookAndFeelChanged() {
  if (auto *lnf = dynamic_cast<Plot::LookAndFeelMethods *>(&getLookAndFeel())) {
    m_lookandfeel = lnf;
    if (getBounds().getWidth() > 0 && getBounds().getHeight() > 0) {
      updateGridInternal(false);
    }
  } else {
    m_lookandfeel = nullptr;
  }
}

void Grid::setXLabels(const std::vector<std::string> &x_labels) {
  m_custom_x_labels = x_labels;
}

void Grid::updateGrid(const bool use_cached_grids) {
  updateGridInternal(use_cached_grids);
}

void Grid::resized() {}

void Grid::setGridType(const GridType grid_type) { m_grid_type = grid_type; }

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
            static_cast<cmp::Plot::LookAndFeelMethods *>(m_lookandfeel)) {
      lnf->updateVerticalGridLineTicksAuto(getBounds(), *m_common_plot_params,
                                           m_grid_type, m_x_prev_ticks,
                                           x_ticks);
      lnf->updateHorizontalGridLineTicksAuto(getBounds(), *m_common_plot_params,
                                             m_grid_type, m_y_prev_ticks,
                                             y_ticks);
      m_x_prev_ticks = x_ticks;
      m_y_prev_ticks = y_ticks;
    }
  }
}
}  // namespace cmp