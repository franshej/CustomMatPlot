
#include "cmp_lookandfeel.h"

#include "cmp_graph_line.h"

/*============================================================================*/

static const [[nodiscard]] std::string getNextCustomLabel(
    std::vector<std::string>::reverse_iterator& custom_labels_it) {
  return *(custom_labels_it++);
}

/*============================================================================*/

namespace cmp {

template <Scaling x_scaling_t, Scaling y_scaling_t>
PlotLookAndFeelDefault<x_scaling_t, y_scaling_t>::PlotLookAndFeelDefault() {
  setDefaultPlotColours();
}

template <Scaling x_scaling_t, Scaling y_scaling_t>
void PlotLookAndFeelDefault<x_scaling_t,
                            y_scaling_t>::setDefaultPlotColours() noexcept {
  setColour(Plot::background_colour, juce::Colour(0xff566573));
  setColour(Plot::frame_colour, juce::Colour(0xffcacfd2));
  setColour(Plot::zoom_frame_colour, juce::Colour(0xff99A3A4));

  setColour(Plot::grid_colour, juce::Colour(0xff99A3A4));
  setColour(Plot::x_grid_label_colour, juce::Colour(0xffaab7b8));
  setColour(Plot::y_grid_label_colour, juce::Colour(0xffaab7b8));

  setColour(Plot::x_label_colour, juce::Colour(0xffecf0f1));
  setColour(Plot::y_label_colour, juce::Colour(0xffecf0f1));
  setColour(Plot::title_label_colour, juce::Colour(0xffecf0f1));
  setColour(Plot::legend_label_colour, juce::Colour(0xffecf0f1));

  setColour(Plot::first_graph_colour, juce::Colour(0xffec7063));
  setColour(Plot::second_graph_colour, juce::Colour(0xffa569Bd));
  setColour(Plot::third_graph_colour, juce::Colour(0xff85c1e9));
  setColour(Plot::fourth_graph_colour, juce::Colour(0xff73c6b6));
  setColour(Plot::fifth_graph_colour, juce::Colour(0xfff4d03f));
  setColour(Plot::sixth_graph_colour, juce::Colour(0xffeB984e));

  setColour(Plot::trace_background_colour, juce::Colour(0xff566573));
  setColour(Plot::trace_label_colour, juce::Colour(0xffecf0f1));
  setColour(Plot::trace_label_frame_colour, juce::Colour(0xffcacfd2));
  setColour(Plot::trace_point_colour, juce::Colour(0xffec7063));
  setColour(Plot::trace_point_frame_colour, juce::Colour(0xff566573));
}

template <Scaling x_scaling_t, Scaling y_scaling_t>
void PlotLookAndFeelDefault<x_scaling_t, y_scaling_t>::drawBackground(
    juce::Graphics& g, juce::Rectangle<int>& bounds){};

template <Scaling x_scaling_t, Scaling y_scaling_t>
juce::Colour
PlotLookAndFeelDefault<x_scaling_t, y_scaling_t>::findAndGetColourFromId(
    const Plot::ColourIdsGraph colour_id) const noexcept {
  return findColour(colour_id);
}

template <Scaling x_scaling_t, Scaling y_scaling_t>
juce::Colour
PlotLookAndFeelDefault<x_scaling_t, y_scaling_t>::findAndGetColourFromId(
    const Plot::ColourIds colour_id) const noexcept {
  return findColour(colour_id);
}

template <Scaling x_scaling_t, Scaling y_scaling_t>
CONSTEXPR20 juce::Rectangle<int>
PlotLookAndFeelDefault<x_scaling_t, y_scaling_t>::getPlotBounds(
    juce::Rectangle<int> bounds) const noexcept {
  return juce::Rectangle<int>(0, 0, bounds.getWidth(), bounds.getHeight());
}

template <Scaling x_scaling_t, Scaling y_scaling_t>
CONSTEXPR20 std::pair<juce::Rectangle<int>, juce::Rectangle<int>>
PlotLookAndFeelDefault<x_scaling_t, y_scaling_t>::getTraceAndZoomButtonBounds(
    juce::Rectangle<int> graph_bounds) const noexcept {
  return {juce::Rectangle<int>(), juce::Rectangle<int>()};
}

template <Scaling x_scaling_t, Scaling y_scaling_t>
juce::Rectangle<int>
PlotLookAndFeelDefault<x_scaling_t, y_scaling_t>::getGraphBounds(
    const juce::Rectangle<int> bounds,
    const juce::Component* const plot_comp) const noexcept {
  const auto estimated_grid_label_width = getGridLabelFont().getStringWidth(
      std::string(getMaximumAllowedCharacterGridLabel(), 'W'));

  auto graph_bounds =
      juce::Rectangle<int>(estimated_grid_label_width, 50,
                           bounds.getWidth() - estimated_grid_label_width * 2,
                           bounds.getHeight() - 125);

  if (const auto* plot = dynamic_cast<const Plot*>(plot_comp)) {
    const auto is_labels_set = plot->getIsLabelsAreSet();
    const auto [x_grid_label_width, y_grid_label_width] =
        plot->getMaxGridLabelWidth();

    auto right = 0;
    auto left = getMargin();
    auto top = getGridLabelFont().getHeight() / 2;
    auto bottom =
        bounds.getHeight() - (getGridLabelFont().getHeight() + getMargin() * 2);

    if (is_labels_set.x_label) {
      bottom -= (getXYTitleFont().getHeight() + getMargin());
    }

    if (is_labels_set.y_label) {
      left = std::size_t(getXYTitleFont().getHeight()) + 2 * getMargin();
    }

    if (is_labels_set.title_label) {
      top = getXYTitleFont().getHeight() + 2 * getMargin();
    }

    if (y_grid_label_width) {
      left += y_grid_label_width + getMargin();
    } else {
      left += estimated_grid_label_width;
    }

    if (x_grid_label_width) {
      right += bounds.getWidth() - x_grid_label_width / 2;
    } else {
      right += bounds.getWidth() - estimated_grid_label_width / 2;
    }

    graph_bounds.setLeft(left);
    graph_bounds.setTop(top);
    graph_bounds.setRight(right);
    graph_bounds.setBottom(bottom);
  }

  return std::move(graph_bounds);
}

template <Scaling x_scaling_t, Scaling y_scaling_t>
CONSTEXPR20 std::size_t PlotLookAndFeelDefault<x_scaling_t, y_scaling_t>::
    getMaximumAllowedCharacterGridLabel() const noexcept {
  return 6u;
};

template <Scaling x_scaling_t, Scaling y_scaling_t>
CONSTEXPR20 juce::Point<int>
PlotLookAndFeelDefault<x_scaling_t, y_scaling_t>::getLegendPosition(
    const juce::Rectangle<int>& graph_bounds,
    const juce::Rectangle<int>& legend_bounds) const noexcept {
  constexpr std::size_t margin_width = 5u;
  constexpr std::size_t margin_height = 5u;

  const auto graph_top_right = graph_bounds.getTopRight();
  const auto x_pos =
      graph_top_right.getX() - legend_bounds.getWidth() - margin_width;
  const auto y_pos = graph_top_right.getY() + margin_height;

  return juce::Point<int>(int(x_pos), int(y_pos));
}

template <Scaling x_scaling_t, Scaling y_scaling_t>
CONSTEXPR20 juce::Rectangle<int>
PlotLookAndFeelDefault<x_scaling_t, y_scaling_t>::getLegendBounds(
    [[maybe_unused]] const juce::Rectangle<int>& graph_bounds,
    const std::vector<std::string>& label_texts) const noexcept {
  constexpr std::size_t margin_width = 5u;
  constexpr std::size_t margin_height = 5u;

  const auto font = getLegendFont();
  const auto height = label_texts.size() * font.getHeightInPoints() +
                      (label_texts.size() + 0.5) * margin_height;

  std::size_t text_width = 0u;
  for (const auto& label : label_texts) {
    const auto new_text_width = std::size_t(font.getStringWidth(label));
    text_width = std::max(new_text_width, text_width);
  }

  const auto width = text_width + 6 * margin_width;

  auto bounds_retval = juce::Rectangle<int>(0, 0, width, height);
  const auto xy_positions = getLegendPosition(graph_bounds, bounds_retval);
  bounds_retval.setPosition(xy_positions);

  return bounds_retval;
}

template <Scaling x_scaling_t, Scaling y_scaling_t>
CONSTEXPR20 juce::Font PlotLookAndFeelDefault<
    x_scaling_t, y_scaling_t>::getLegendFont() const noexcept {
  return juce::Font(14.0f, juce::Font::plain);
}

template <Scaling x_scaling_t, Scaling y_scaling_t>
CONSTEXPR20 juce::Font PlotLookAndFeelDefault<
    x_scaling_t, y_scaling_t>::getButtonFont() const noexcept {
  return juce::Font(14.0f, juce::Font::plain);
}

template <Scaling x_scaling_t, Scaling y_scaling_t>
CONSTEXPR20 Plot::ColourIdsGraph
PlotLookAndFeelDefault<x_scaling_t, y_scaling_t>::getColourFromGraphID(
    const std::size_t graph_id) const {
  /**< Colour vector which is useful when iterating over the six graph
   * colours.*/
  static const std::vector<Plot::ColourIdsGraph> GraphColours{
      Plot::ColourIdsGraph::first_graph_colour,
      Plot::ColourIdsGraph::second_graph_colour,
      Plot::ColourIdsGraph::third_graph_colour,
      Plot::ColourIdsGraph::fourth_graph_colour,
      Plot::ColourIdsGraph::fifth_graph_colour,
      Plot::ColourIdsGraph::sixth_graph_colour};

  return GraphColours[graph_id % GraphColours.size()];
}

template <Scaling x_scaling_t, Scaling y_scaling_t>
CONSTEXPR20 std::size_t
PlotLookAndFeelDefault<x_scaling_t, y_scaling_t>::getMargin() const noexcept {
  return 8u;
}

template <Scaling x_scaling_t, Scaling y_scaling_t>
CONSTEXPR20 std::size_t PlotLookAndFeelDefault<
    x_scaling_t, y_scaling_t>::getMarkerLength() const noexcept {
  return 20u;
}

template <Scaling x_scaling_t, Scaling y_scaling_t>
std::pair<juce::Rectangle<int>, juce::Rectangle<int>>
PlotLookAndFeelDefault<x_scaling_t, y_scaling_t>::getTraceXYLabelBounds(
    const std::string_view x_text, const std::string_view y_text) const {
  const auto margin = getMargin();
  const auto font = getTraceFont();

  const auto width_X = 2 * margin + font.getStringWidth(x_text.data());
  const auto width_Y = 2 * margin + font.getStringWidth(y_text.data());
  const auto height_XY = font.getHeight();
  const auto x_coord_XY = margin;
  const auto y_coord_X = margin;
  const auto y_coord_Y = 2 * margin + font.getHeight();

  const auto x_label_bounds =
      juce::Rectangle<int>(x_coord_XY, y_coord_X, width_X, height_XY);
  const auto y_label_bounds =
      juce::Rectangle<int>(x_coord_XY, y_coord_Y, width_Y, height_XY);

  return {x_label_bounds, y_label_bounds};
}

template <Scaling x_scaling_t, Scaling y_scaling_t>
CONSTEXPR20 juce::Rectangle<int>
PlotLookAndFeelDefault<x_scaling_t, y_scaling_t>::getTraceLabelLocalBounds(
    const juce::Rectangle<int>& x_label_bounds,
    const juce::Rectangle<int>& y_label_bounds) const noexcept {
  const auto width =
      std::max(x_label_bounds.getWidth(), y_label_bounds.getWidth());
  const auto height = y_label_bounds.getBottom() + getMargin();
  const auto retval = juce::Rectangle<int>(0, 0, width, height);
  return retval;
}

template <Scaling x_scaling_t, Scaling y_scaling_t>
CONSTEXPR20 juce::Rectangle<int> PlotLookAndFeelDefault<
    x_scaling_t, y_scaling_t>::getTracePointLocalBounds() const noexcept {
  return juce::Rectangle<int>(0, 0, 10, 10);
}

template <Scaling x_scaling_t, Scaling y_scaling_t>
CONSTEXPR20 juce::Font PlotLookAndFeelDefault<
    x_scaling_t, y_scaling_t>::getTraceFont() const noexcept {
  return juce::Font(14.0f, juce::Font::plain);
}

template <Scaling x_scaling_t, Scaling y_scaling_t>
CONSTEXPR20 juce::Point<int>
PlotLookAndFeelDefault<x_scaling_t, y_scaling_t>::getTracePointPositionFrom(
    const CommonPlotParameterView common_plot_params,
    const juce::Point<float> graph_values) const noexcept {
  const auto [x_scale, x_offset] =
      getXScaleAndOffset(common_plot_params.graph_bounds.getWidth(),
                         common_plot_params.x_lim, getXScaling());

  const auto [y_scale, y_offset] =
      getYScaleAndOffset(common_plot_params.graph_bounds.getHeight(),
                         common_plot_params.y_lim, getYScaling());

  float x;
  float y;

  if constexpr (m_x_scaling == Scaling::linear) {
    x = getXGraphValueLinear(graph_values.getX(), x_scale, x_offset);
  } else {
    x = getXGraphPointsLogarithmic(graph_values.getX(), x_scale, x_offset);
  }
  if constexpr (m_y_scaling == Scaling::linear) {
    y = getYGraphValueLinear(graph_values.getY(), y_scale, y_offset);
  } else {
    y = getYGraphPointsLogarithmic(graph_values.getY(), y_scale, y_offset);
  }

  return juce::Point<float>(x, y).toInt();
}

template <Scaling x_scaling_t, Scaling y_scaling_t>
void PlotLookAndFeelDefault<x_scaling_t, y_scaling_t>::drawGraphLine(
    juce::Graphics& g, const GraphLineDataView graph_line_data) {
  juce::Path graph_path;
  const juce::PathStrokeType stroke_type =
      graph_line_data.graph_attribute.path_stroke_type
          ? graph_line_data.graph_attribute.path_stroke_type.value()
          : juce::PathStrokeType(1.0f,
                                 juce::PathStrokeType::JointStyle::mitered,
                                 juce::PathStrokeType::EndCapStyle::rounded);

  const auto& graph_points = graph_line_data.graph_points;
  const auto& dashed_lengths = graph_line_data.graph_attribute.dashed_lengths;
  const auto& marker = graph_line_data.graph_attribute.marker;
  auto graph_colour = graph_line_data.graph_attribute.graph_colour.value();

  if (graph_line_data.graph_attribute.graph_line_opacity) {
    graph_colour = graph_colour.withAlpha(
        graph_line_data.graph_attribute.graph_line_opacity.value());
  }

  if (graph_points.size() > 1) {
    graph_path.startNewSubPath(graph_points[0]);
    std::for_each(
        graph_points.begin() + 1, graph_points.end(),
        [&](const juce::Point<float>& point) { graph_path.lineTo(point); });

    if (dashed_lengths) {
      stroke_type.createDashedStroke(graph_path, graph_path,
                                     dashed_lengths.value().data(),
                                     int(dashed_lengths.value().size()));
    }

    g.setColour(graph_colour);

    g.strokePath(graph_path, stroke_type);

    if (marker) {
      const auto marker_length = float(getMarkerLength());

      const auto marker_path =
          Marker::getMarkerPathFrom(marker.value(), marker_length);

      for (const auto& point : graph_points) {
        auto path = marker_path;

        path.applyTransform(
            juce::AffineTransform::translation(point.getX(), point.getY()));

        if (marker.value().FaceColour) {
          g.setColour(marker.value().FaceColour.value());

          g.fillPath(path);
        }

        if (marker.value().EdgeColour) {
          g.setColour(marker.value().EdgeColour.value());
        } else {
          g.setColour(graph_colour);
        }

        g.strokePath(path, marker.value().edge_stroke_type);
      }
    }
  }
}

template <Scaling x_scaling_t, Scaling y_scaling_t>
void PlotLookAndFeelDefault<x_scaling_t, y_scaling_t>::drawGridLabels(
    juce::Graphics& g, const LabelVector& x_axis_labels,
    const LabelVector& y_axis_labels) {
  g.setColour(findColour(Plot::x_grid_label_colour));

  g.setFont(getGridLabelFont());
  for (const auto& x_axis_text : x_axis_labels) {
    g.drawText(x_axis_text.first, x_axis_text.second,
               juce::Justification::centred);
  }
  for (const auto& y_axis_text : y_axis_labels) {
    g.drawText(y_axis_text.first, y_axis_text.second,
               juce::Justification::centredRight);
  }
}

template <Scaling x_scaling_t, Scaling y_scaling_t>
void PlotLookAndFeelDefault<x_scaling_t, y_scaling_t>::drawFrame(
    juce::Graphics& g, const juce::Rectangle<int> bounds) {
  g.setColour(findColour(Plot::frame_colour));

  const juce::Rectangle<int> frame = {0, 0, bounds.getWidth(),
                                      bounds.getHeight()};
  g.drawRect(frame);
}

template <Scaling x_scaling_t, Scaling y_scaling_t>
void PlotLookAndFeelDefault<x_scaling_t, y_scaling_t>::drawGridLine(
    juce::Graphics& g, const GridLine& grid_line, const bool grid_on) {
  constexpr auto margin = 8.f;
  const auto y_and_len = grid_line.length + grid_line.position.getY();
  const auto x_and_len = grid_line.length + grid_line.position.getX();

  switch (grid_line.direction) {
    g.setColour(findColour(Plot::grid_colour));
    case GridLine::Direction::vertical:

      if (grid_on) {
        g.drawVerticalLine(float(grid_line.position.getX()),
                           float(grid_line.position.getY()), y_and_len);
      } else {
        g.drawVerticalLine(float(grid_line.position.getX()),
                           float(grid_line.position.getY()),
                           float(grid_line.position.getY()) + margin);

        g.drawVerticalLine(float(grid_line.position.getX()), y_and_len - margin,
                           y_and_len);
      }

      break;
    case GridLine::Direction::horizontal:

      if (grid_on) {
        g.drawHorizontalLine(float(grid_line.position.getY()),
                             float(grid_line.position.getX()), x_and_len);
      } else {
        g.drawHorizontalLine(float(grid_line.position.getY()),
                             float(grid_line.position.getX()),
                             float(grid_line.position.getX()) + margin);
        g.drawHorizontalLine(float(grid_line.position.getY()),
                             x_and_len - margin, x_and_len);
      }
      break;
    default:
      break;
  }
}

template <Scaling x_scaling_t, Scaling y_scaling_t>
void PlotLookAndFeelDefault<x_scaling_t, y_scaling_t>::drawLegend(
    juce::Graphics& g, const StringVector& label_texts,
    const std::vector<juce::Colour>& graph_line_colours,
    const juce::Rectangle<int>& bounds) {
  jassert_return(
      graph_line_colours.size() == label_texts.size(),
      "\'label_texts\' and \'graph_line_colours\ must have the same size.'");

  constexpr std::size_t margin_width = 5u;
  constexpr std::size_t margin_height = 5u;
  const auto font = getLegendFont();

  const juce::Rectangle<int> frame = {0, 0, bounds.getWidth(),
                                      bounds.getHeight()};

  g.setFont(getLegendFont());

  const auto height = int(font.getHeightInPoints());
  int i = 0u;
  for (const auto label : label_texts) {
    const auto width = font.getStringWidth(label);
    const auto x = margin_width;
    const int y = i * (height + margin_height) + margin_height;
    juce::Rectangle<int> label_bounds = {x, y, width, height};

    g.setColour(findColour(Plot::legend_label_colour));
    g.drawText(label, label_bounds, juce::Justification::centredLeft);
    g.setColour(graph_line_colours[i]);
    g.fillRect(x + width + margin_width, y + height / 2, margin_width * 2, 2);
    i++;
  }

  g.setColour(findColour(Plot::frame_colour));
  g.drawRect(frame);
}

template <Scaling x_scaling_t, Scaling y_scaling_t>
void PlotLookAndFeelDefault<x_scaling_t, y_scaling_t>::drawSpread(
    juce::Graphics& g, const GraphLine* first_graph,
    const GraphLine* second_graph, const juce::Colour& spread_colour) {
  juce::Path path;

  if (!first_graph->getGraphPoints().empty()) {
    const auto& first_graph_points = first_graph->getGraphPoints();
    const auto& second_graph_points = second_graph->getGraphPoints();

    path.startNewSubPath(first_graph_points[0]);

    std::for_each(first_graph_points.begin() + 1, first_graph_points.end(),
                  [&](const auto point) { path.lineTo(point); });

    std::for_each(second_graph_points.rbegin(), second_graph_points.rend(),
                  [&](const auto point) { path.lineTo(point); });

    path.closeSubPath();

    g.setColour(spread_colour);
    g.fillPath(path);
  }
}

template <Scaling x_scaling_t, Scaling y_scaling_t>
void PlotLookAndFeelDefault<x_scaling_t, y_scaling_t>::drawTraceLabel(
    juce::Graphics& g, const cmp::Label& x_label, const cmp::Label& y_label) {
  g.setColour(findColour(Plot::trace_label_colour));
  g.setFont(getTraceFont());
  g.drawText(x_label.first, x_label.second, juce::Justification::left);
  g.drawText(y_label.first, y_label.second, juce::Justification::left);

  const auto frame_width =
      std::max(x_label.second.getWidth(), y_label.second.getWidth());
  const auto frame_height =
      x_label.second.getHeight() + y_label.second.getHeight() + getMargin() * 3;
  const auto frame_bounds =
      juce::Rectangle<int>(0, 0, frame_width, frame_height);
  g.setColour(findColour(Plot::trace_label_frame_colour));
  g.drawRect(frame_bounds);
};

template <Scaling x_scaling_t, Scaling y_scaling_t>
void PlotLookAndFeelDefault<x_scaling_t, y_scaling_t>::drawTracePoint(
    juce::Graphics& g, const juce::Rectangle<int>& bounds) {
  constexpr int line_thickness = 4;

  const auto x = bounds.getX() + line_thickness / 2;
  const auto y = bounds.getY() + line_thickness / 2;
  const auto w = bounds.getWidth() - line_thickness;
  const auto h = bounds.getHeight() - line_thickness;

  if (!bounds.isEmpty()) {
    g.setColour(findColour(Plot::trace_point_colour));
    g.drawEllipse(x, y, w, h, line_thickness);
  }
}

template <Scaling x_scaling_t, Scaling y_scaling_t>
void PlotLookAndFeelDefault<x_scaling_t, y_scaling_t>::drawZoomArea(
    juce::Graphics& g, juce::Point<int>& start_coordinates,
    const juce::Point<int>& end_coordinates,
    const juce::Rectangle<int>& graph_bounds) noexcept {
  const auto dx = start_coordinates.getX() - end_coordinates.getX();
  const auto dy = start_coordinates.getY() - end_coordinates.getY();

  auto width{0};
  auto height{0};

  auto x{0};
  auto y{0};

  if (dx < 0) {
    x = start_coordinates.getX();
  } else {
    x = start_coordinates.getX() - dx;
  }

  if (dy < 0) {
    y = start_coordinates.getY();
  } else {
    y = start_coordinates.getY() - dy;
  }

  if (end_coordinates.getX() > graph_bounds.getRight()) {
    width = abs(start_coordinates.getX() - graph_bounds.getRight());
  } else if (end_coordinates.getX() < graph_bounds.getX()) {
    width = abs(start_coordinates.getX() - graph_bounds.getX());
    x = graph_bounds.getX();
  } else {
    width = abs(dx);
  }

  if (end_coordinates.getY() > graph_bounds.getBottom()) {
    height = abs(start_coordinates.getY() - graph_bounds.getBottom());
  } else if (end_coordinates.getY() < graph_bounds.getY()) {
    height = abs(start_coordinates.getY() - graph_bounds.getY());
    y = graph_bounds.getY();
  } else {
    height = abs(dy);
  }

  const auto zoom_area = juce::Rectangle<int>(x, y, width, height);

  juce::Path path;
  path.addRectangle(zoom_area);
  const juce::PathStrokeType pathStrokType(1.0);

  constexpr float dashedLengh[2] = {4, 4};
  pathStrokType.createDashedStroke(path, path, dashedLengh, 2);

  g.setColour(findColour(Plot::zoom_frame_colour));
  g.strokePath(path, pathStrokType);
}

template <Scaling x_scaling_t, Scaling y_scaling_t>
void PlotLookAndFeelDefault<x_scaling_t, y_scaling_t>::
    updateXGraphPointsAndIndices(const juce::Rectangle<int>& bounds,
                                 const Lim_f& x_lim,
                                 const std::vector<float>& x_data,
                                 std::vector<std::size_t>& graph_points_indices,
                                 GraphPoints& graph_points) noexcept {
  graph_points_indices.resize(x_data.size());
  graph_points.resize(x_data.size());

  const auto width = static_cast<float>(bounds.getWidth());
  const auto [x_scale, x_offset] =
      getXScaleAndOffset(width, x_lim, m_x_scaling);

  std::size_t min_x_index{0u}, max_x_index{x_data.size() - 1u};

  // If the graph has less than 10 values we simply plot all points even if
  // they are on top of each other.
  if (x_data.size() < 10u) {
    std::iota(graph_points_indices.begin(), graph_points_indices.end(), 0u);
    goto calculate_x_label;
  }

  // The points that are on top of each other or outside the graph area are
  // dicarded.
  for (const auto& x : x_data) {
    if (x >= x_lim.min) {
      auto start_i_outside = 2u;
      while (true) {
        if ((int(min_x_index) - start_i_outside) >= 0) {
          // start_i_outside indices outside the left side to get rid of
          // artifacts.
          min_x_index = min_x_index - start_i_outside;
          break;
        }
        --start_i_outside;
      }
      break;
    }
    min_x_index++;
  }

  for (auto x = x_data.rbegin(); x != x_data.rend(); ++x) {
    if (*x <= x_lim.max || max_x_index == 0u) {
      auto start_i_outside = 2u;
      while (true) {
        if (max_x_index < x_data.size() - start_i_outside) {
          // Two indices outside the right side to get rid of artifacts.
          max_x_index = max_x_index + start_i_outside;
          break;
        }
        --start_i_outside;
      }
      break;
    }
    max_x_index--;
  }

  if (min_x_index >= max_x_index) {
    min_x_index = 0;
    max_x_index = x_data.size() - 1u;
    graph_points_indices.resize(max_x_index - min_x_index);
  }

  graph_points_indices.front() = min_x_index;

  if (max_x_index - min_x_index > 1u) {
    float last_added_x = std::numeric_limits<float>::min();
    std::size_t current_index = min_x_index;
    std::size_t graph_point_index{0u};
    const auto inverse_x_scale = 1.f / x_scale;

    if constexpr (m_x_scaling == Scaling::linear) {
      for (auto x = x_data.begin() + min_x_index;
           x != x_data.begin() + max_x_index; ++x) {
        if (abs(*x - last_added_x) > inverse_x_scale) {
          last_added_x = *x;
          graph_points_indices[graph_point_index++] = current_index;
        }
        current_index++;
      }
    } else if constexpr (m_x_scaling == Scaling::logarithmic) {
      for (auto x = x_data.begin() + min_x_index;
           x != x_data.begin() + max_x_index; ++x) {
        if (log10(abs(*x / last_added_x)) > inverse_x_scale) {
          last_added_x = *x;
          graph_points_indices[graph_point_index++] = current_index;
        }
        current_index++;
      }
    }

    graph_points_indices.resize(graph_point_index + 1);
  }

  graph_points_indices.back() = max_x_index;

calculate_x_label:

  graph_points.resize(graph_points_indices.size());

  std::size_t i{0u};
  if constexpr (m_x_scaling == Scaling::linear) {
    for (const auto i_x : graph_points_indices) {
      graph_points[i++].setX(
          getXGraphValueLinear(x_data[i_x], x_scale, x_offset));
    }
  } else if constexpr (m_x_scaling == Scaling::logarithmic) {
    for (const auto i_x : graph_points_indices) {
      graph_points[i++].setX(
          getXGraphPointsLogarithmic(x_data[i_x], x_scale, x_offset));
    }
  }
}

template <Scaling x_scaling_t, Scaling y_scaling_t>
void PlotLookAndFeelDefault<x_scaling_t, y_scaling_t>::updateYGraphPoints(
    const juce::Rectangle<int>& bounds, const Lim_f& y_lim,
    const std::vector<float>& y_data,
    const std::vector<std::size_t>& graph_points_indices,
    GraphPoints& graph_points) noexcept {
  const auto [y_scale, y_offset] =
      getYScaleAndOffset(bounds.toFloat().getHeight(), y_lim, m_y_scaling);

  std::size_t i = 0u;

  if constexpr (m_y_scaling == Scaling::linear) {
    for (const auto i_y : graph_points_indices) {
      graph_points[i].setY(
          getYGraphValueLinear(y_data[i_y], y_scale, y_offset));
      i++;
    }
  } else if constexpr (m_y_scaling == Scaling::logarithmic) {
    for (const auto i_y : graph_points_indices) {
      graph_points[i].setY(
          getYGraphPointsLogarithmic(y_data[i_y], y_scale, y_offset));
      i++;
    }
  }
}

template <Scaling x_scaling_t, Scaling y_scaling_t>
void PlotLookAndFeelDefault<x_scaling_t, y_scaling_t>::
    updateVerticalGridLineTicksAuto(const juce::Rectangle<int>& bounds,
                                    const bool tiny_grids, const Lim_f x_lim,
                                    std::vector<float>& x_ticks) noexcept {
  x_ticks.clear();

  const auto width = bounds.getWidth();
  const auto height = bounds.getHeight();

  const auto addVerticalTicksLinear = [&]() {
    auto num_vertical_lines = 3u;
    if (width > 435u) {
      num_vertical_lines = 11u;
    } else if (width <= 435u && width > 175u) {
      num_vertical_lines = 5u;
    }
    num_vertical_lines =
        tiny_grids ? std::size_t(num_vertical_lines * 1.5) : num_vertical_lines;

    auto x_diff = (x_lim.max - x_lim.min) / float(num_vertical_lines);
    for (std::size_t i = 0; i != num_vertical_lines + 1u; ++i) {
      const auto x_pos = x_lim.min + float(i) * x_diff;
      x_ticks.push_back(x_pos);
    }
  };

  const auto addVerticalTicksLogarithmic = [&]() {
    auto num_lines_per_power = 3u;
    if (width > 435u) {
      num_lines_per_power = 10u;
    } else if (width <= 435u && width > 175u) {
      num_lines_per_power = 5u;
    }
    num_lines_per_power = tiny_grids ? std::size_t(num_lines_per_power * 1.5)
                                     : num_lines_per_power;

    const auto min_power = std::floor(log10(x_lim.min));
    const auto max_power = std::ceil(log10(x_lim.max));

    const auto power_diff = ceil(abs(max_power) - abs(min_power));

    // Frist create the vertical lines
    for (float curr_power = min_power; curr_power < max_power; ++curr_power) {
      const auto curr_x_pos_base = pow(10.f, curr_power);

      const auto x_diff =
          pow(10.f, curr_power + 1.f) / static_cast<float>(num_lines_per_power);

      float last_tick = std::numeric_limits<float>::min();
      for (float line = 0; line < num_lines_per_power; ++line) {
        const auto x_tick =
            floor((curr_x_pos_base + line * x_diff) / curr_x_pos_base) *
            curr_x_pos_base;
        if (last_tick != x_tick) {
          x_ticks.push_back(x_tick);
        }
      }
    }
  };

  if constexpr (m_x_scaling == Scaling::linear) {
    addVerticalTicksLinear();
  } else if constexpr (m_x_scaling == Scaling::logarithmic) {
    addVerticalTicksLogarithmic();
  }
}

template <Scaling x_scaling_t, Scaling y_scaling_t>
void PlotLookAndFeelDefault<x_scaling_t, y_scaling_t>::
    updateHorizontalGridLineTicksAuto(const juce::Rectangle<int>& bounds,
                                      const bool tiny_grids, const Lim_f y_lim,
                                      std::vector<float>& y_ticks) noexcept {
  y_ticks.clear();

  const auto width = bounds.getWidth();
  const auto height = bounds.getHeight();

  const auto addHorizontalTicksLinear = [&]() {
    auto num_horizontal_lines = 3u;
    if (height > 375u) {
      num_horizontal_lines = 11u;
    } else if (height <= 375u && height > 135u) {
      num_horizontal_lines = 5u;
    }
    num_horizontal_lines = tiny_grids ? std::size_t(num_horizontal_lines * 1.5)
                                      : num_horizontal_lines;

    auto y_diff = (y_lim.max - y_lim.min) / float(num_horizontal_lines);
    for (std::size_t i = 0u; i != num_horizontal_lines + 1u; ++i) {
      const auto y_pos = y_lim.min + float(i) * y_diff;
      y_ticks.push_back(y_pos);
    }
  };

  const auto addHorizontalTicksLogarithmic = [&]() {
    auto num_lines_per_power = 3u;
    if (height > 375u) {
      num_lines_per_power = 11u;
    } else if (height <= 375u && height > 135u) {
      num_lines_per_power = 5u;
    }
    num_lines_per_power = tiny_grids ? std::size_t(num_lines_per_power * 1.5)
                                     : num_lines_per_power;

    const auto min_power = std::floor(log10(y_lim.min));
    const auto max_power = std::ceil(log10(y_lim.max));

    const auto power_diff = ceil(abs(max_power) - abs(min_power));

    for (float curr_power = min_power; curr_power < max_power; ++curr_power) {
      const auto curr_y_pos_base = pow(10.f, curr_power);

      const auto y_diff =
          pow(10.f, curr_power + 1.f) / static_cast<float>(num_lines_per_power);

      constexpr float last_tick = std::numeric_limits<float>::min();
      for (float line = 0; line < num_lines_per_power; ++line) {
        const auto y_tick =
            std::floor((curr_y_pos_base + line * y_diff) / curr_y_pos_base) *
            curr_y_pos_base;
        if (y_tick != last_tick) {
          y_ticks.push_back(y_tick);
        }
      }
    }
  };

  if constexpr (m_y_scaling == Scaling::linear) {
    addHorizontalTicksLinear();
  } else if constexpr (m_y_scaling == Scaling::logarithmic) {
    addHorizontalTicksLogarithmic();
  }
}

template <Scaling x_scaling_t, Scaling y_scaling_t>
CONSTEXPR20 juce::Font PlotLookAndFeelDefault<
    x_scaling_t, y_scaling_t>::getGridLabelFont() const noexcept {
  return juce::Font("Arial Rounded MT", 16.f, juce::Font::plain);
}

template <Scaling x_scaling_t, Scaling y_scaling_t>
CONSTEXPR20 juce::Font PlotLookAndFeelDefault<
    x_scaling_t, y_scaling_t>::getXYTitleFont() const noexcept {
  return juce::Font(20.0f, juce::Font::plain);
}

template <Scaling x_scaling_t, Scaling y_scaling_t>
CONSTEXPR20 Scaling
PlotLookAndFeelDefault<x_scaling_t, y_scaling_t>::getXScaling() const noexcept {
  return m_x_scaling;
};

template <Scaling x_scaling_t, Scaling y_scaling_t>
CONSTEXPR20 Scaling
PlotLookAndFeelDefault<x_scaling_t, y_scaling_t>::getYScaling() const noexcept {
  return m_y_scaling;
};

template <Scaling x_scaling_t, Scaling y_scaling_t>
void PlotLookAndFeelDefault<x_scaling_t, y_scaling_t>::updateGridLabels(
    const CommonPlotParameterView common_plot_params,
    const std::vector<GridLine>& grid_lines, StringVector& x_custom_label_ticks,
    StringVector& y_custom_label_ticks, LabelVector& x_axis_labels_out,
    LabelVector& y_axis_labels_out) {
  const auto [x, y, width, height] =
      getRectangleMeasures<int>(common_plot_params.graph_bounds);
  const auto font = getGridLabelFont();

  const auto num_horizonal_lines =
      std::count_if(grid_lines.begin(), grid_lines.end(), [](const auto& gl) {
        return gl.direction == GridLine::Direction::horizontal;
      });

  const auto num_vertical_lines =
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
                ? getNextCustomLabel(custom_x_labels_reverse_it)
                : valueToString(tick, common_plot_params, true).first;

        const auto [label_width, label_height] =
            getLabelWidthAndHeight(font, label);

        const auto bound = juce::Rectangle<int>(
            int(position.x) - label_width / 2,
            common_plot_params.graph_bounds.getBottom() + getMargin(),
            label_width, label_height);

        checkInterectionWithLastLabelAndAdd(x_last_label_bound,
                                            x_axis_labels_out, label, bound);
      } break;
      case GridLine::Direction::horizontal: {
        const auto label =
            use_custom_y_labels
                ? getNextCustomLabel(custom_y_labels_reverse_it)
                : valueToString(tick, common_plot_params, false).first;

        const auto [label_width, label_height] =
            getLabelWidthAndHeight(font, label);

        const auto bound = juce::Rectangle<int>(
            x - (label_width + getMargin()), int(position.y) - label_height / 2,
            label_width, label_height);

        checkInterectionWithLastLabelAndAdd(y_last_label_bound,
                                            y_axis_labels_out, label, bound);
      } break;
      default:
        break;
    }
  }
}

template <Scaling x_scaling_t, Scaling y_scaling_t>
void PlotLookAndFeelDefault<x_scaling_t, y_scaling_t>::updateXYTitleLabels(
    const juce::Rectangle<int>& bounds,
    const juce::Rectangle<int>& graph_bounds, juce::Label& x_label,
    juce::Label& y_label, juce::Label& title_label) {
  const auto font = getXYTitleFont();

  const auto x_margin = int(getMargin());
  const auto y_margin = int(getMargin());
  const auto title_margin = int(getMargin());

  const auto y_label_width = font.getStringWidth(x_label.getText());
  const auto x_label_width = font.getStringWidth(y_label.getText());
  const auto title_width = font.getStringWidth(title_label.getText());
  const auto font_height = int(font.getHeightInPoints());

  x_label.setFont(font);
  y_label.setFont(font);
  title_label.setFont(font);

  x_label.setJustificationType(juce::Justification::centred);
  y_label.setJustificationType(juce::Justification::centred);
  title_label.setJustificationType(juce::Justification::centred);

  x_label.setColour(juce::Label::textColourId,
                    findColour(Plot::x_label_colour));
  y_label.setColour(juce::Label::textColourId,
                    findColour(Plot::y_label_colour));
  title_label.setColour(juce::Label::textColourId,
                        findColour(Plot::title_label_colour));

  const juce::Rectangle<int> y_area = {
      y_margin, graph_bounds.getY() + graph_bounds.getHeight() / 2 + y_margin,
      y_label_width, font_height};

  y_label.setTransform(juce::AffineTransform::rotation(
      -juce::MathConstants<float>::halfPi, y_area.getX(), y_area.getY()));

  const auto y_mid_point_bottom =
      (bounds.getHeight() - (graph_bounds.getY() + graph_bounds.getHeight())) /
      2;

  y_label.setBounds(y_area);

  x_label.setBounds(
      graph_bounds.getX() + graph_bounds.getWidth() / 2 - x_margin,
      graph_bounds.getBottom() + getGridLabelFont().getHeight() +
          2 * getMargin(),
      x_label_width, font_height);

  title_label.setBounds(
      graph_bounds.getX() + graph_bounds.getWidth() / 2 - title_margin,
      graph_bounds.getY() - (title_margin + font.getHeight()), title_width,
      font_height);
}

template class PlotLookAndFeelDefault<Scaling::linear, Scaling::linear>;
template class PlotLookAndFeelDefault<Scaling::linear, Scaling::logarithmic>;
template class PlotLookAndFeelDefault<Scaling::logarithmic, Scaling::linear>;
template class PlotLookAndFeelDefault<Scaling::logarithmic,
                                      Scaling::logarithmic>;
}  // namespace cmp