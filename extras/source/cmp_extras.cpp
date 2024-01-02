#include "cmp_extras.hpp"

#include "cmp_utils.h"

namespace cmp {

////////////////////////////////////////////////////////////////////////////////
///////////////////////// PlotLookAndFeelTimeline //////////////////////////////
////////////////////////////////////////////////////////////////////////////////

PlotLookAndFeelTimeline::PlotLookAndFeelTimeline() : PlotLookAndFeel() {
  overridePlotColours();
}

juce::Rectangle<int> PlotLookAndFeelTimeline::getGraphBounds(
    const juce::Rectangle<int> bounds,
    const juce::Component* const plot_comp) const noexcept {
  const auto estimated_grid_label_width = getGridLabelFont().getStringWidth(
      std::string(getMaximumAllowedCharacterGridLabel(), 'W'));

  auto graph_bounds = juce::Rectangle<int>();

  if (const auto* plot = dynamic_cast<const Plot*>(plot_comp)) {
    const auto is_labels_set = getIsLabelsAreSet(plot);
    const auto [x_grid_label_width, y_grid_label_width] =
        getMaxGridLabelWidth(plot);

    auto top = getMarginSmall() * 2 + getGridLabelFont().getHeight();

    const auto is_x_axis_label_below_graph = isXAxisLabelsBelowGraph();
    auto bottom =
        is_x_axis_label_below_graph
            ? bounds.getHeight() -
                  (getGridLabelFont().getHeight() + getMargin() +
                   getXGridLabelDistanceFromGraphBound())
            : bounds.getHeight() -
                  (getMargin() + getXGridLabelDistanceFromGraphBound());

    if (is_labels_set.x_label) {
      bottom -= (getXYTitleFont().getHeight() + getMargin());
    }

    if (is_labels_set.title_label) {
      top += getXYTitleFont().getHeight() + getMargin();
    }

    graph_bounds.setLeft(0);
    graph_bounds.setTop(int(top));
    graph_bounds.setRight(bounds.getWidth());
    graph_bounds.setBottom(bounds.getHeight());
  }

  return std::move(graph_bounds);
}

void PlotLookAndFeelTimeline::drawFrame(juce::Graphics& g [[maybe_unused]],
                                        const juce::Rectangle<int> bounds
                                        [[maybe_unused]]) {}

void PlotLookAndFeelTimeline::updateGridLabels(
    const CommonPlotParameterView common_plot_params,
    const std::vector<GridLine>& grid_lines, StringVector& x_custom_label_ticks,
    StringVector& y_custom_label_ticks, LabelVector& x_axis_labels_out,
    LabelVector& y_axis_labels_out) {
  const auto [x, y, width, height] =
      getRectangleMeasures<int>(common_plot_params.graph_bounds);
  const auto font = getGridLabelFont();

  const std::size_t num_horizonal_lines =
      std::count_if(grid_lines.begin(), grid_lines.end(), [](const auto& gl) {
        return gl.direction == GridLine::Direction::horizontal;
      });

  const std::size_t num_vertical_lines =
      std::count_if(grid_lines.begin(), grid_lines.end(), [](const auto& gl) {
        return gl.direction == GridLine::Direction::vertical;
      });

  juce::Rectangle<int>* x_last_label_bound = nullptr;
  juce::Rectangle<int>* y_last_label_bound = nullptr;

  x_axis_labels_out.clear();
  y_axis_labels_out.clear();

  const auto use_custom_x_labels = x_custom_label_ticks.size() > 0u;
  const auto use_custom_y_labels = y_custom_label_ticks.size() > 0u;

  auto custom_x_labels_reverse_it = x_custom_label_ticks.rbegin();
  if (use_custom_x_labels) {
    if (x_custom_label_ticks.size() >= num_vertical_lines) {
      custom_x_labels_reverse_it = x_custom_label_ticks.rbegin() +
                                   x_custom_label_ticks.size() -
                                   num_vertical_lines;
    } else {
      x_custom_label_ticks.resize(num_vertical_lines);

      custom_x_labels_reverse_it = x_custom_label_ticks.rbegin();
    }
  }

  auto custom_y_labels_reverse_it = y_custom_label_ticks.rbegin();
  if (use_custom_y_labels) {
    if (y_custom_label_ticks.size() >= num_horizonal_lines) {
      custom_y_labels_reverse_it = y_custom_label_ticks.rbegin() +
                                   y_custom_label_ticks.size() -
                                   num_horizonal_lines;
    } else {
      y_custom_label_ticks.resize(num_horizonal_lines);

      custom_y_labels_reverse_it = y_custom_label_ticks.rbegin();
    }
  }

  const auto checkInterectionWithLastLabelAndAdd =
      [&](auto& last_rect, auto& axis_labels, const auto& label,
          const auto& bound) {
        if (!last_rect) {
          axis_labels.push_back({label, bound});
          last_rect = &axis_labels.back().second;
        } else {
          if (!last_rect->intersects(bound)) {
            axis_labels.push_back({label, bound});
            last_rect = &axis_labels.back().second;
          }
        }
      };

  const auto getLabelWidthAndHeight =
      [](const auto& font, const auto& label) -> std::pair<int, int> {
    return std::make_pair(font.getStringWidth(label),
                          int(font.getHeightInPoints()));
  };

  for (auto it = grid_lines.rbegin(); it != grid_lines.rend(); it++) {
    const auto position = it->position;
    const auto direction = it->direction;
    const auto tick = it->tick;

    switch (direction) {
      case GridLine::Direction::vertical: {
        const auto label =
            use_custom_x_labels
                ? getNextCustomLabel(custom_x_labels_reverse_it,
                                     x_custom_label_ticks.rend())
                : valueToString(tick, common_plot_params, true).first;

        const auto [label_width, label_height] =
            getLabelWidthAndHeight(font, label);

        const auto is_x_axis_label_below_graph = isXAxisLabelsBelowGraph();
        const auto bound_y =
            is_x_axis_label_below_graph
                ? common_plot_params.graph_bounds.getBottom() +
                      getXGridLabelDistanceFromGraphBound()
                : common_plot_params.graph_bounds.getTopLeft().y -
                      label_height - getMarginSmall() / 2;

        const auto bound =
            juce::Rectangle<int>(int(position.x) - label_width / 2, bound_y,
                                 label_width, label_height);

        checkInterectionWithLastLabelAndAdd(x_last_label_bound,
                                            x_axis_labels_out, label, bound);
      } break;
      case GridLine::Direction::horizontal: {
        const auto label =
            use_custom_y_labels
                ? getNextCustomLabel(custom_y_labels_reverse_it,
                                     y_custom_label_ticks.rend())
                : valueToString(tick, common_plot_params, false).first;

        const auto [label_width, label_height] =
            getLabelWidthAndHeight(font, label);

        const auto bound = juce::Rectangle<int>(
            x + getMarginSmall() * 2, int(position.y) - label_height / 2,
            label_width, label_height);

        checkInterectionWithLastLabelAndAdd(y_last_label_bound,
                                            y_axis_labels_out, label, bound);
      } break;
      default:
        break;
    }
  }
}

bool PlotLookAndFeelTimeline::isXAxisLabelsBelowGraph() const noexcept {
  return false;
}

void PlotLookAndFeelTimeline::drawGridLabels(juce::Graphics& g,
                                             const LabelVector& x_axis_labels,
                                             const LabelVector& y_axis_labels) {
  g.setColour(findColour(Plot::x_grid_label_colour));
  g.setFont(getGridLabelFont());
  for (const auto& x_axis_text : x_axis_labels) {
    g.drawText(x_axis_text.first, x_axis_text.second,
               juce::Justification::centred);
  }
  g.setColour(findColour(Plot::y_grid_label_colour));
  for (const auto& y_axis_text : y_axis_labels) {
    g.drawText(y_axis_text.first, y_axis_text.second,
               juce::Justification::centredLeft);
  }
}

juce::Font PlotLookAndFeelTimeline::getGridLabelFont() const noexcept {
  return juce::Font("Arial Rounded MT", 10.f, juce::Font::plain);
}

void PlotLookAndFeelTimeline::updateVerticalGridLineTicksAuto(
    const juce::Rectangle<int>& bounds,
    const CommonPlotParameterView& common_plot_parameter_view,
    const GridType grid_type, const std::vector<float>& previous_ticks,
    std::vector<float>& x_ticks) noexcept {
  x_ticks.clear();

  const auto x_min = common_plot_parameter_view.x_lim.min;
  const auto x_max = common_plot_parameter_view.x_lim.max;

  const auto dx = x_max - x_min > 100.f ? 10.f : 5.f;

  const auto start_value = floor(x_min / dx) * dx;
  const auto end_value = ceil(x_max / dx) * dx;

  for (auto x = start_value; x <= end_value; x += dx) {
    x_ticks.push_back(x);
  }
}

void PlotLookAndFeelTimeline::updateHorizontalGridLineTicksAuto(
    const juce::Rectangle<int>& bounds,
    const CommonPlotParameterView& common_plot_parameter_view,
    const GridType grid_type, const std::vector<float>& previous_ticks,
    std::vector<float>& y_ticks) noexcept {
  y_ticks.clear();

  const auto y_min = common_plot_parameter_view.y_lim.min;
  const auto y_max = common_plot_parameter_view.y_lim.max;

  const auto dy = y_max - y_min > 100.f ? 10.f : 5.f;

  const auto start_value = floor(y_min / dy) * dy;
  const auto end_value = ceil(y_max / dy) * dy;

  for (auto y = start_value; y <= end_value; y += dy) {
    y_ticks.push_back(y);
  }
}

std::size_t PlotLookAndFeelTimeline::getMarginSmall() const noexcept {
  return 2u;
}

void PlotLookAndFeelTimeline::drawBackground(
    juce::Graphics& g, const juce::Rectangle<int>& bounds) {}

void PlotLookAndFeelTimeline::overridePlotColours() noexcept {
  setColour(Plot::grid_colour, juce::Colour(0xff181818));
  setColour(Plot::transluent_grid_colour, juce::Colour(0xff252525));
}
}  // namespace cmp