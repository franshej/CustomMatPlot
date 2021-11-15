#pragma once

#include "spl_plot.h"

/*============================================================================*/

template <class float_type>
constexpr std::string convertFloatToString(const float_type value,
                                           std::size_t num_decimals,
                                           std::size_t max_string_len) {
  if (!(std::is_same<float, float_type>::value ||
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

std::string getNextCustomLabel(
    std::vector<std::string>::reverse_iterator& custom_labels_it) {
  return *(custom_labels_it++);
}

/*============================================================================*/

namespace scp {
class PlotLookAndFeel : public juce::LookAndFeel_V3,
                        public PlotBase::LookAndFeelMethods {
 public:
  PlotLookAndFeel() { setDefaultPlotColours(); }

  virtual ~PlotLookAndFeel() override{};

  void setDefaultPlotColours() noexcept override {
    setColour(PlotBase::background_colour, juce::Colour(0xff566573));
    setColour(PlotBase::frame_colour, juce::Colour(0xffcacfd2));

    setColour(PlotBase::grid_colour, juce::Colour(0xff99A3A4));
    setColour(PlotBase::x_grid_label_colour, juce::Colour(0xffaab7b8));
    setColour(PlotBase::y_grid_label_colour, juce::Colour(0xffaab7b8));
    setColour(PlotBase::zoom_area_colour, juce::Colour(0xff99A3A4));

    setColour(PlotBase::x_label_colour, juce::Colour(0xffecf0f1));
    setColour(PlotBase::y_label_colour, juce::Colour(0xffecf0f1));
    setColour(PlotBase::title_label_colour, juce::Colour(0xffecf0f1));
    setColour(PlotBase::legend_label_colour, juce::Colour(0xffecf0f1));

    setColour(PlotBase::first_graph_colour, juce::Colour(0xffec7063));
    setColour(PlotBase::second_graph_colour, juce::Colour(0xffa569Bd));
    setColour(PlotBase::third_graph_colour, juce::Colour(0xff85c1e9));
    setColour(PlotBase::fourth_graph_colour, juce::Colour(0xff73c6b6));
    setColour(PlotBase::fifth_graph_colour, juce::Colour(0xfff4d03f));
    setColour(PlotBase::sixth_graph_colour, juce::Colour(0xffeB984e));
  }

  juce::Colour findAndGetColourFromId(
      const PlotBase::ColourIdsGraph colour_id) const noexcept override {
    return findColour(colour_id);
  }

  juce::Colour findAndGetColourFromId(
      const PlotBase::ColourIds colour_id) const noexcept override {
    return findColour(colour_id);
  }

  juce::Rectangle<int> getPlotBounds(
      juce::Rectangle<int>& bounds) const noexcept override {
    return juce::Rectangle<int>(0, 0, bounds.getWidth(), bounds.getHeight());
  }

  juce::Rectangle<int> getGraphBounds(
      const juce::Rectangle<int>& bounds) const noexcept override {
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

  PlotBase::ColourIdsGraph getColourFromGraphID(
      const std::size_t graph_id) const override {
    /**< Colour vector which is useful when iterating over the six graph
     * colours.*/
    static const std::vector<PlotBase::ColourIdsGraph> GraphColours{
        PlotBase::ColourIdsGraph::first_graph_colour,
        PlotBase::ColourIdsGraph::second_graph_colour,
        PlotBase::ColourIdsGraph::third_graph_colour,
        PlotBase::ColourIdsGraph::fourth_graph_colour,
        PlotBase::ColourIdsGraph::fifth_graph_colour,
        PlotBase::ColourIdsGraph::sixth_graph_colour};

    return GraphColours[graph_id % GraphColours.size()];
  }

  void drawGraphLine(juce::Graphics& g, const GraphPoints& graph_points,
                     const std::vector<float>& dashed_lengths,
                     const GraphType graph_type,
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
      switch (graph_type) {
        case GraphType::graph_line:
          g.setColour(graph_colour);
          break;
        case GraphType::grid_line:
          g.setColour(findColour(PlotBase::ColourIds::grid_colour));
          break;
        default:
          g.setColour(juce::Colours::pink);
          break;
      }

      g.strokePath(graph_path, p_type);
    }
  }

  void drawGridLabels(juce::Graphics& g, const LabelVector& x_axis_labels,
                      const LabelVector& y_axis_labels) override {
    g.setColour(findColour(PlotBase::x_grid_label_colour));
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
    g.setColour(findColour(PlotBase::frame_colour));

    const juce::Rectangle<int> frame = {0, 0, bounds.getWidth(),
                                        bounds.getHeight()};
    g.drawRect(frame);
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

      g.setColour(findColour(PlotBase::legend_label_colour));
      g.drawText(label, label_bounds, juce::Justification::centredLeft);
      g.setColour(graph_line_colours[i]);
      g.fillRect(x + width + margin_width, y + height / 2, margin_width * 2, 2);
      i++;
    }

    g.setColour(findColour(PlotBase::frame_colour));
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
    juce::PathStrokeType pathStrokType(1.0);

    float dashedLengh[2];
    dashedLengh[0] = 4;
    dashedLengh[1] = 4;
    pathStrokType.createDashedStroke(path, path, dashedLengh, 2);

    g.setColour(findColour(PlotBase::zoom_area_colour));
    g.strokePath(path, pathStrokType);
  }

  void updateXGraphPointsAndIndices(
      const juce::Rectangle<int>& bounds, const Scaling scaling,
      const Lim_f& x_lim, const std::vector<float>& x_data,
      std::vector<std::size_t>& graph_points_indices,
      GraphPoints& graph_points) noexcept override {
    graph_points_indices.resize(0);
    graph_points_indices.reserve(x_data.size());

    graph_points.resize(x_data.size());

    const auto width = static_cast<float>(bounds.getWidth());
    const auto [x_scale, x_offset] = getXScaleAndOffset(width, x_lim, scaling);

    std::size_t min_x_index{0u};
    for (const auto& x : x_data) {
      if (x >= x_lim.min) {
        if (min_x_index) {
          min_x_index = min_x_index - 1;
        }
        break;
      }
      min_x_index++;
    }

    std::size_t max_x_index{x_data.size() - 1u};
    for (auto x = x_data.rbegin(); x != x_data.rend(); ++x) {
      if (*x <= x_lim.max || max_x_index == 0u) {
        if (max_x_index < x_data.size() - 1u) {
          max_x_index = max_x_index + 1;
        }
        break;
      }
      max_x_index--;
    }

    if (min_x_index >= max_x_index) {
      min_x_index = 0;
      max_x_index = x_data.size() - 1u;
    }

    graph_points_indices.push_back(min_x_index);

    float last_added_x = std::numeric_limits<float>::min();
    if (max_x_index - min_x_index > 1) {
      std::size_t current_index = min_x_index;
      for (auto x = x_data.begin() + min_x_index + 1;
           x != x_data.begin() + max_x_index - 1; ++x) {
        if (abs(*x - last_added_x) > 1 / x_scale) {
          last_added_x = *x;
          graph_points_indices.push_back(current_index);
        }
        current_index++;
      }
    }
    graph_points_indices.push_back(max_x_index);

    std::size_t i{0u};
    switch (scaling) {
      case Scaling::linear:
        for (const auto i_x : graph_points_indices) {
          graph_points[i].setX(
              getXGraphPointsLinear(x_data[i_x], x_scale, x_offset));
          i++;
        }
        break;
      case Scaling::logarithmic:
        for (const auto i_x : graph_points_indices) {
          graph_points[i].setX(
              getXGraphPointsLogarithmic(x_data[i_x], x_scale, x_offset));
          i++;
        }
        break;
      default:
        break;
    }
  }

  void updateYGraphPoints(const juce::Rectangle<int>& bounds,
                          const Scaling scaling, const Lim_f& y_lim,
                          const std::vector<float>& y_data,
                          const std::vector<std::size_t>& graph_points_indices,
                          GraphPoints& graph_points) noexcept override {
    const auto [y_scale, y_offset] =
        getYScaleAndOffset(bounds.toFloat().getHeight(), y_lim, scaling);

    std::size_t i = 0u;

    switch (scaling) {
      case Scaling::linear:
        for (const auto i_x : graph_points_indices) {
          graph_points[i].setY(
              getYGraphPointsLinear(y_data[i_x], y_scale, y_offset));
          i++;
        }
        break;
      case Scaling::logarithmic:
        break;
      default:
        break;
    }

    graph_points.resize(i);
  }

  void updateVerticalGridLineTicksAuto(
      const juce::Rectangle<int>& bounds, const Scaling vertical_scaling,
      const bool tiny_grids, const Lim_f x_lim,
      std::vector<float>& x_ticks) noexcept override {
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

      // Create the vertical lines
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
        for (float line = 0; line < num_lines_per_power; ++line) {
          const auto x_tick = curr_x_pos_base + line * x_diff;
          x_ticks.push_back(x_tick);
        }
      }
    };

    switch (vertical_scaling) {
      case Scaling::linear:
        addVerticalTicksLinear();
        break;
      case Scaling::logarithmic:
        addVerticalTicksLogarithmic();
        break;
      default:
        addVerticalTicksLinear();
        break;
    }
  }

  void updateHorizontalGridLineTicksAuto(
      const juce::Rectangle<int>& bounds, const Scaling hotizontal_scaling,
      const bool tiny_grids, const Lim_f y_lim,
      std::vector<float>& y_ticks) noexcept override {
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

      // Then create the horizontal lines
      auto y_diff = (y_lim.max - y_lim.min) / float(num_horizontal_lines);
      for (std::size_t i = 0u; i != num_horizontal_lines + 1u; ++i) {
        const auto y_pos = y_lim.min + float(i) * y_diff;
        y_ticks.push_back(y_pos);
      }
    };

    switch (hotizontal_scaling) {
      case Scaling::linear:
        addHorizontalTicksLinear();
        break;
      case Scaling::logarithmic:
        jassert_return(false, "Not implmeneted yet.");
        break;
      default:
        addHorizontalTicksLinear();
        break;
    }
  }

  juce::Font getGridLabelFont() const noexcept override {
    return juce::Font("Arial Rounded MT", 16.f, juce::Font::plain);
  }

  juce::Font getXYTitleFont() const noexcept override {
    return juce::Font(20.0f, juce::Font::plain);
  }

  void updateGridLabelsVertical(const juce::Rectangle<int>& bounds,
                                const GridLines& vertical_grid_lines,
                                const std::vector<float>& custom_x_ticks,
                                StringVector& custom_x_labels,
                                LabelVector& x_axis_labels) override {
    const auto grid_area = getGraphBounds(bounds);
    const auto [x, y, width, height] =
        scp::getRectangleMeasures<int>(grid_area);

    const auto font = getGridLabelFont();

    juce::Rectangle<int>* x_last_rect = nullptr;

    x_axis_labels.clear();

    const auto use_custom_x_labels =
        custom_x_labels.size() >= vertical_grid_lines.size();

    auto custom_x_labels_reverse_it =
        use_custom_x_labels
            ? std::make_reverse_iterator(custom_x_labels.begin()) +
                  custom_x_ticks.size() - vertical_grid_lines.size()
            : std::make_reverse_iterator(custom_x_labels.begin());

    std::for_each(std::make_reverse_iterator(vertical_grid_lines.end()),
                  std::make_reverse_iterator(vertical_grid_lines.begin()),
                  [&](const auto& grid) {
                    const auto x_val = grid->getXValues();
                    const auto graph_points = grid->getGraphPoints();
                    const auto x_pos = graph_points[0].x;

                    const std::string x_label =
                        use_custom_x_labels
                            ? getNextCustomLabel(custom_x_labels_reverse_it)
                            : convertFloatToString(x_val[0], 2, 6);

                    const auto x_label_width = font.getStringWidth(x_label);
                    const auto font_height = int(font.getHeightInPoints());

                    const auto x_label_area = juce::Rectangle<int>(
                        x + int(x_pos) - x_label_width / 2,
                        y + height + font_height, x_label_width, font_height);

                    if (!x_last_rect) {
                      x_axis_labels.push_back({x_label, x_label_area});
                      x_last_rect = &x_axis_labels.back().second;
                    } else {
                      if (!x_last_rect->intersects(x_label_area)) {
                        x_axis_labels.push_back({x_label, x_label_area});
                        x_last_rect = &x_axis_labels.back().second;
                      }
                    }
                  });
  }

  void updateGridLabelsHorizontal(const juce::Rectangle<int>& bounds,
                                  const GridLines& horizontal_grid_lines,
                                  const std::vector<float>& custom_y_ticks,
                                  StringVector& custom_y_labels,
                                  LabelVector& y_axis_labels) override {
    const auto grid_area = getGraphBounds(bounds);
    const auto [x, y, width, height] =
        scp::getRectangleMeasures<int>(grid_area);

    const auto font = getGridLabelFont();
    juce::Rectangle<int>* y_last_rect = nullptr;

    y_axis_labels.clear();

    const auto use_custom_y_labels =
        custom_y_labels.size() >= horizontal_grid_lines.size();

    auto custom_y_labels_reverse_it =
        use_custom_y_labels
            ? std::make_reverse_iterator(custom_y_labels.begin()) +
                  custom_y_ticks.size() - horizontal_grid_lines.size()
            : std::make_reverse_iterator(custom_y_labels.begin());

    std::for_each(std::make_reverse_iterator(horizontal_grid_lines.end()),
                  std::make_reverse_iterator(horizontal_grid_lines.begin()),
                  [&](const auto& grid) {
                    const auto y_val = grid->getYValues();
                    const auto graph_points = grid->getGraphPoints();
                    const auto y_pos = graph_points[0].y;

                    const std::string y_label =
                        use_custom_y_labels
                            ? getNextCustomLabel(custom_y_labels_reverse_it)
                            : convertFloatToString(y_val[0], 2, 6);

                    const auto y_label_width = font.getStringWidth(y_label);
                    const auto font_height = int(font.getHeightInPoints());

                    auto y_label_area =
                        juce::Rectangle<int>(x - font_height - y_label_width,
                                             y + int(y_pos) - font_height / 2,
                                             y_label_width, font_height);

                    if (!y_last_rect) {
                      y_axis_labels.push_back({y_label, y_label_area});
                      y_last_rect = &y_axis_labels.back().second;
                    } else {
                      if (!y_last_rect->intersects(y_label_area)) {
                        y_axis_labels.push_back({y_label, y_label_area});
                        y_last_rect = &y_axis_labels.back().second;
                      }
                    }
                  });
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
                      findColour(PlotBase::x_label_colour));
    y_label.setColour(juce::Label::textColourId,
                      findColour(PlotBase::y_label_colour));
    title_label.setColour(juce::Label::textColourId,
                          findColour(PlotBase::title_label_colour));

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
};  // class PlotLookAndFeel
};  // namespace scp