#pragma once

#include <array>

#include "spl_plot.h"

/*============================================================================*/

template <class float_type>
static constexpr [[nodiscard]] const std::string convertFloatToString(
    const float_type value, std::size_t num_decimals,
    std::size_t max_string_len) {
  if constexpr (!(std::is_same<float, float_type>::value ||
                  std::is_same<double, float_type>::value)) {
    throw std::invalid_argument("Type must be either float or double");
  }
  const auto pow_of_ten = value == 0.f ? 0 : int(floor(log10(abs(value))));
  const auto is_neg = std::size_t(value < 0);

  auto text_out = std::to_string(value);

  const auto len_before_dec = pow_of_ten < 0
                                  ? std::size_t(abs(float(pow_of_ten)))
                                  : std::size_t(pow_of_ten) + 1u;
  const auto req_len = len_before_dec + is_neg + num_decimals + 1 /* 1 = dot */;

  if (max_string_len < req_len) {
    if (pow_of_ten >= 0) {
      const auto two_decimals =
          text_out.substr(is_neg + std::size_t(pow_of_ten) + 1u, 3);
      const auto first_digit = text_out.substr(0, 1u + is_neg);
      text_out = first_digit + two_decimals + "e" + std::to_string(pow_of_ten);
    } else {
      auto three_decimals = text_out.substr(len_before_dec + is_neg + 1u, 4);
      three_decimals.insert(0, ".");
      text_out = std::to_string(-1 * is_neg) + three_decimals + "e" +
                 std::to_string(pow_of_ten);
    }
  } else {
    text_out = text_out.substr(0, len_before_dec + is_neg + 1u + num_decimals);
  }

  return text_out;
}

static const [[nodiscard]] std::string getNextCustomLabel(
    std::vector<std::string>::reverse_iterator& custom_labels_it) {
  return *(custom_labels_it++);
}

/*============================================================================*/

namespace scp {

class PlotLookAndFeel;

template <Scaling x_scaling_t, Scaling y_scaling_t>
class PlotLookAndFeelDefault : public Plot::LookAndFeelMethods {
  static constexpr auto x_scaling = x_scaling_t;
  static constexpr auto y_scaling = y_scaling_t;

 public:
  PlotLookAndFeelDefault() { setDefaultPlotColours(); }

  void setDefaultPlotColours() noexcept override {
    setColour(Plot::background_colour, juce::Colour(0xff566573));
    setColour(Plot::frame_colour, juce::Colour(0xffcacfd2));

    setColour(Plot::grid_colour, juce::Colour(0xff99A3A4));
    setColour(Plot::x_grid_label_colour, juce::Colour(0xffaab7b8));
    setColour(Plot::y_grid_label_colour, juce::Colour(0xffaab7b8));
    setColour(Plot::zoom_area_colour, juce::Colour(0xff99A3A4));

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
  }

  juce::Colour findAndGetColourFromId(
      const Plot::ColourIdsGraph colour_id) const noexcept override {
    return findColour(colour_id);
  }

  juce::Colour findAndGetColourFromId(
      const Plot::ColourIds colour_id) const noexcept override {
    return findColour(colour_id);
  }

  juce::Rectangle<int> getPlotBounds(
      juce::Rectangle<int> bounds) const noexcept override {
    return juce::Rectangle<int>(0, 0, bounds.getWidth(), bounds.getHeight());
  }

  juce::Rectangle<int> getGraphBounds(
      const juce::Rectangle<int> bounds) const noexcept override {
    return juce::Rectangle<int>(100, 50, bounds.getWidth() - 150,
                                bounds.getHeight() - 125);
  }

  juce::Point<int> getLegendPosition(
      const juce::Rectangle<int>& graph_bounds,
      const juce::Rectangle<int>& legend_bounds) const noexcept override {
    constexpr std::size_t margin_width = 5u;
    constexpr std::size_t margin_height = 5u;

    const auto graph_top_right = graph_bounds.getTopRight();
    const auto x_pos =
        graph_top_right.getX() - legend_bounds.getWidth() - margin_width;
    const auto y_pos = graph_top_right.getY() + margin_height;

    return juce::Point<int>(x_pos, y_pos);
  }

  juce::Rectangle<int> getLegendBounds(
      [[maybe_unused]] const juce::Rectangle<int>& graph_bounds,
      const std::vector<std::string>& label_texts) const noexcept override {
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

  juce::Font getLegendFont() const noexcept override {
    return juce::Font(14.0f, juce::Font::plain);
  }

  Plot::ColourIdsGraph getColourFromGraphID(
      const std::size_t graph_id) const override {
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

  void drawGraphLine(juce::Graphics& g, const GraphPoints& graph_points,
                     const std::vector<float>& dashed_lengths,
                     const juce::Colour graph_colour) override {
    juce::Path graph_path;
    juce::PathStrokeType p_type(1.0f, juce::PathStrokeType::JointStyle::mitered,
                                juce::PathStrokeType::EndCapStyle::rounded);

    if (graph_points.size() > 1) {
      graph_path.startNewSubPath(graph_points[0]);
      std::for_each(
          graph_points.begin() + 1, graph_points.end(),
          [&](const juce::Point<float>& point) { graph_path.lineTo(point); });

      if (!dashed_lengths.empty()) {
        p_type.createDashedStroke(graph_path, graph_path, dashed_lengths.data(),
                                  int(dashed_lengths.size()));
      }

      g.setColour(graph_colour);
      g.strokePath(graph_path, p_type);
    }
  }

  void drawGridLabels(juce::Graphics& g, const LabelVector& x_axis_labels,
                      const LabelVector& y_axis_labels) override {
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

  void drawFrame(juce::Graphics& g,
                 const juce::Rectangle<int> bounds) override {
    g.setColour(findColour(Plot::frame_colour));

    const juce::Rectangle<int> frame = {0, 0, bounds.getWidth(),
                                        bounds.getHeight()};
    g.drawRect(frame);
  }

  void drawGridLine(juce::Graphics& g, const GridLine& grid_line,
                    const bool grid_on) override {
    constexpr auto margin = 8;
    const auto y_and_len = grid_line.length + grid_line.position.getY();
    const auto x_and_len = grid_line.length + grid_line.position.getX();

    switch (grid_line.direction) {
      g.setColour(findColour(Plot::grid_colour));
      case GridLine::Direction::vertical:

        if (grid_on) {
          g.drawVerticalLine(grid_line.position.getX(),
                             grid_line.position.getY(), y_and_len);
        } else {
          g.drawVerticalLine(grid_line.position.getX(),
                             grid_line.position.getY(),
                             grid_line.position.getY() + margin);

          g.drawVerticalLine(grid_line.position.getX(), y_and_len - margin,
                             y_and_len);
        }

        break;
      case GridLine::Direction::horizontal:

        if (grid_on) {
          g.drawHorizontalLine(grid_line.position.getY(),
                               grid_line.position.getX(), x_and_len);
        } else {
          g.drawHorizontalLine(grid_line.position.getY(),
                               grid_line.position.getX(),
                               grid_line.position.getX() + margin);
          g.drawHorizontalLine(grid_line.position.getY(), x_and_len - margin,
                               x_and_len);
        }
        break;
      default:
        break;
    }
  }

  void drawLegend(juce::Graphics& g, const StringVector& label_texts,
                  const std::vector<juce::Colour>& graph_line_colours,
                  const juce::Rectangle<int>& bounds) override {
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

  void drawZoomArea(
      juce::Graphics& g, juce::Point<int>& start_coordinates,
      const juce::Point<int>& end_coordinates,
      const juce::Rectangle<int>& graph_bounds) noexcept override {
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

    g.setColour(findColour(Plot::zoom_area_colour));
    g.strokePath(path, pathStrokType);
  }

  void updateXGraphPointsAndIndices(
      const juce::Rectangle<int>& bounds, const Lim_f& x_lim,
      const std::vector<float>& x_data,
      std::vector<std::size_t>& graph_points_indices,
      GraphPoints& graph_points) noexcept override {
    graph_points_indices.resize(x_data.size());
    graph_points.resize(x_data.size());

    const auto width = static_cast<float>(bounds.getWidth());
    const auto [x_scale, x_offset] =
        getXScaleAndOffset(width, x_lim, x_scaling);

    std::size_t min_x_index{0u}, max_x_index{x_data.size() - 1u};

    if (x_data.size() == 1u) {
      goto calculate_x_label;
    }

    for (const auto& x : x_data) {
      if (x >= x_lim.min) {
        if (min_x_index - 1u) {
          // Two indices outside the left side to get rid of artifacts.
          min_x_index = min_x_index - 2u;
        }
        break;
      }
      min_x_index++;
    }

    for (auto x = x_data.rbegin(); x != x_data.rend(); ++x) {
      if (*x <= x_lim.max || max_x_index == 0u) {
        if (max_x_index < x_data.size() - 2u) {
          // Two indices outside the right side to get rid of artifacts.
          max_x_index = max_x_index + 2;
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

      if constexpr (x_scaling == Scaling::linear) {
        for (auto x = x_data.begin() + min_x_index + 1;
             x != x_data.begin() + max_x_index - 1; ++x) {
          if (abs(*x - last_added_x) > inverse_x_scale) {
            last_added_x = *x;
            graph_points_indices[graph_point_index++] = current_index;
          }
          current_index++;
        }
      } else if constexpr (x_scaling == Scaling::logarithmic) {
        for (auto x = x_data.begin() + min_x_index + 1;
             x != x_data.begin() + max_x_index - 1; ++x) {
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

    std::size_t i{0u};
    if constexpr (x_scaling == Scaling::linear) {
      for (const auto i_x : graph_points_indices) {
        graph_points[i].setX(
            getXGraphPointsLinear(x_data[i_x], x_scale, x_offset));
        i++;
      }
    } else if constexpr (x_scaling == Scaling::logarithmic) {
      for (const auto i_x : graph_points_indices) {
        graph_points[i].setX(
            getXGraphPointsLogarithmic(x_data[i_x], x_scale, x_offset));
        i++;
      }
    }
  }

  void updateYGraphPoints(const juce::Rectangle<int>& bounds,
                          const Lim_f& y_lim, const std::vector<float>& y_data,
                          const std::vector<std::size_t>& graph_points_indices,
                          GraphPoints& graph_points) noexcept override {
    const auto [y_scale, y_offset] =
        getYScaleAndOffset(bounds.toFloat().getHeight(), y_lim, y_scaling);

    std::size_t i = 0u;

    if constexpr (y_scaling == Scaling::linear) {
      for (const auto i_y : graph_points_indices) {
        graph_points[i].setY(
            getYGraphPointsLinear(y_data[i_y], y_scale, y_offset));
        i++;
      }
    } else if constexpr (y_scaling == Scaling::logarithmic) {
      for (const auto i_y : graph_points_indices) {
        graph_points[i].setY(
            getYGraphPointsLogarithmic(y_data[i_y], y_scale, y_offset));
        i++;
      }
    }

    graph_points.resize(i);
  }

  void updateVerticalGridLineTicksAuto(
      const juce::Rectangle<int>& bounds, const bool tiny_grids,
      const Lim_f x_lim, std::vector<float>& x_ticks) noexcept override {
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
      num_vertical_lines = tiny_grids ? std::size_t(num_vertical_lines * 1.5)
                                      : num_vertical_lines;

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

        const auto x_diff = pow(10.f, curr_power + 1.f) /
                            static_cast<float>(num_lines_per_power);

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

    if constexpr (x_scaling == Scaling::linear) {
      addVerticalTicksLinear();
    } else if constexpr (x_scaling == Scaling::logarithmic) {
      addVerticalTicksLogarithmic();
    }
  }

  void updateHorizontalGridLineTicksAuto(
      const juce::Rectangle<int>& bounds, const bool tiny_grids,
      const Lim_f y_lim, std::vector<float>& y_ticks) noexcept override {
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
      num_horizontal_lines = tiny_grids
                                 ? std::size_t(num_horizontal_lines * 1.5)
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

        const auto y_diff = pow(10.f, curr_power + 1.f) /
                            static_cast<float>(num_lines_per_power);

        float last_tick = std::numeric_limits<float>::min();
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

    if constexpr (y_scaling == Scaling::linear) {
      addHorizontalTicksLinear();
    } else if constexpr (y_scaling == Scaling::logarithmic) {
      addHorizontalTicksLogarithmic();
    }
  }

  juce::Font getGridLabelFont() const noexcept override {
    return juce::Font("Arial Rounded MT", 16.f, juce::Font::plain);
  }

  juce::Font getXYTitleFont() const noexcept override {
    return juce::Font(20.0f, juce::Font::plain);
  }

  Scaling getXScaling() const noexcept override { return x_scaling; };

  Scaling getYScaling() const noexcept override { return y_scaling; };

  void updateGridLabels(const juce::Rectangle<int>& bounds,
                        const std::vector<GridLine>& grid_lines,
                        StringVector& x_custom_label_ticks,
                        StringVector& y_custom_label_ticks,
                        LabelVector& x_axis_labels_out,
                        LabelVector& y_axis_labels_out) override {
    const auto grid_area = getGraphBounds(bounds);
    const auto [x, y, width, height] = getRectangleMeasures<int>(grid_area);
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
                  : convertFloatToString(tick, 2, 6);

          const auto [label_width, label_height] =
              getLabelWidthAndHeight(font, label);

          const auto bound = juce::Rectangle<int>(
              int(position.x) - label_width / 2, y + height + label_height,
              label_width, label_height);

          checkInterectionWithLastLabelAndAdd(x_last_label_bound,
                                              x_axis_labels_out, label, bound);
        } break;
        case GridLine::Direction::horizontal: {
          const auto label =
              use_custom_y_labels
                  ? getNextCustomLabel(custom_y_labels_reverse_it)
                  : convertFloatToString(tick, 2, 6);

          const auto [label_width, label_height] =
              getLabelWidthAndHeight(font, label);

          const auto bound = juce::Rectangle<int>(
              x - label_height - label_width,
              int(position.y) - label_height / 2, label_width, label_height);

          checkInterectionWithLastLabelAndAdd(y_last_label_bound,
                                              y_axis_labels_out, label, bound);
        } break;
        default:
          break;
      }
    }
  }

  void updateXYTitleLabels(const juce::Rectangle<int>& bounds,
                           juce::Label& x_label, juce::Label& y_label,
                           juce::Label& title_label) override {
    const auto font = getXYTitleFont();
    const auto graph_area = getGraphBounds(bounds);

    constexpr auto x_margin = 50;
    constexpr auto y_margin = 25;
    constexpr auto title_margin = 25;

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
        graph_area.getX() / 2 - y_margin,
        graph_area.getY() + graph_area.getHeight() / 2 + y_margin,
        y_label_width, font_height};

    y_label.setTransform(juce::AffineTransform::rotation(
        -juce::MathConstants<float>::halfPi, y_area.getX(), y_area.getY()));

    const auto y_mid_point_bottom =
        (bounds.getHeight() - (graph_area.getY() + graph_area.getHeight())) / 2;

    y_label.setBounds(y_area);

    x_label.setBounds(
        graph_area.getX() + graph_area.getWidth() / 2 - x_margin,
        graph_area.getY() + graph_area.getHeight() + y_mid_point_bottom,
        x_label_width, font_height);

    title_label.setBounds(
        graph_area.getX() + graph_area.getWidth() / 2 - title_margin,
        graph_area.getY() / 2 - title_margin / 2, title_width, font_height);
  }
};  // class PlotLookAndFeelDefault

class PlotLookAndFeel
    : public PlotLookAndFeelDefault<Scaling::linear, Scaling::linear>,
      public PlotLookAndFeelDefault<Scaling::logarithmic, Scaling::linear>,
      public PlotLookAndFeelDefault<Scaling::linear, Scaling::logarithmic>,
      public PlotLookAndFeelDefault<Scaling::logarithmic,
                                    Scaling::logarithmic> {};

};  // namespace scp