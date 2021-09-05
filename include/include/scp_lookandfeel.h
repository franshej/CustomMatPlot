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
                        public Plot::LookAndFeelMethods {
 public:
  PlotLookAndFeel() { setDefaultPlotColours(); }

  virtual ~PlotLookAndFeel() override{};

  void setDefaultPlotColours() noexcept override {
    setColour(Plot::background_colour, juce::Colour(0xff566573));
    setColour(Plot::frame_colour, juce::Colour(0xffcacfd2));

    setColour(Plot::grid_colour, juce::Colour(0xff99A3A4));
    setColour(Plot::x_grid_label_colour, juce::Colour(0xffaab7b8));
    setColour(Plot::y_grid_label_colour, juce::Colour(0xffaab7b8));

    setColour(Plot::title_label_colour, juce::Colour(0xffecf0f1));
    setColour(Plot::x_label_colour, juce::Colour(0xffecf0f1));
    setColour(Plot::y_label_colour, juce::Colour(0xffecf0f1));

    setColour(Plot::first_graph_colour, juce::Colour(0xffec7063));
    setColour(Plot::second_graph_colour, juce::Colour(0xffa569Bd));
    setColour(Plot::third_graph_colour, juce::Colour(0xff85c1e9));
    setColour(Plot::fourth_graph_colour, juce::Colour(0xff73c6b6));
    setColour(Plot::fifth_graph_colour, juce::Colour(0xfff4d03f));
    setColour(Plot::sixth_graph_colour, juce::Colour(0xffeB984e));
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

  void drawGraphLine(juce::Graphics& g,
                     const std::vector<juce::Point<float>>& graph_points,
                     const std::vector<float>& dashed_lengths,
                     const Plot::GraphType graph_type,
                     const std::size_t graph_id) override {
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
        case Plot::GraphType::GraphLine:
          g.setColour(findColour(getColourFromGraphID(graph_id)));
          break;
        case Plot::GraphType::GridLine:
          g.setColour(findColour(Plot::ColourIds::grid_colour));
          break;
        default:
          g.setColour(juce::Colours::pink);
          break;
      }

      g.strokePath(graph_path, p_type);
    }
  }

  void drawGridLabels(juce::Graphics& g, const LabelVector& x_axis_labels,
                      const LabelVector& y_axis_labels) {
    g.setColour(findColour(Plot::ColourIds::x_grid_label_colour));
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

  void updateXGraphPoints(const juce::Rectangle<int>& bounds,
                          const Plot::Scaling scaling, const Lim_f& x_lim,
                          const std::vector<float>& x_data,
                          GraphPoints& graph_points) noexcept override {
    const auto addXGraphPointsLinear = [&]() {
      const auto x_scale =
          static_cast<float>(bounds.getWidth()) / (x_lim.max - x_lim.min);
      const auto offset_x = static_cast<float>(-(x_lim.min * x_scale));

      std::size_t i = 0u;
      for (const auto& x : x_data) {
        graph_points[i].setX(offset_x + (x * x_scale));
        i++;
      }
    };

    const auto addXGraphPointsLogarithmic = [&]() {
      const auto width = static_cast<float>(bounds.getWidth());

      auto xToXPos = [&](const float x) {
        return width * (log(x / x_lim.min) / log(x_lim.max / x_lim.min));
      };

      std::size_t i = 0u;
      for (const auto& x : x_data) {
        graph_points[i].setX(xToXPos(x));
        i++;
      }
    };

    switch (scaling) {
      case Plot::Scaling::linear:
        addXGraphPointsLinear();
        break;
      case Plot::Scaling::logarithmic:
        addXGraphPointsLogarithmic();
        break;
      default:
        addXGraphPointsLinear();
        break;
    };
  }

  void updateYGraphPoints(const juce::Rectangle<int>& bounds,
                          const Plot::Scaling scaling, const Lim_f& y_lim,
                          const std::vector<float>& y_data,
                          GraphPoints& graph_points) noexcept override {
    const auto addYGraphPointsLinear = [&]() {
      const auto y_scale =
          static_cast<float>(bounds.getHeight()) / (y_lim.max - y_lim.min);
      const auto y_offset = y_lim.min;

      const auto offset_y = float(bounds.getHeight()) + (y_offset * y_scale);

      std::size_t i = 0u;
      for (const auto& y : y_data) {
        graph_points[i].setY(offset_y - (y * y_scale));
        i++;
      }
    };

    switch (scaling) {
      case Plot::Scaling::linear:
        addYGraphPointsLinear();
        break;
      case Plot::Scaling::logarithmic:
        jassert_return(false, "Log scale for y axis is not implemented.");
        break;
      default:
        addYGraphPointsLinear();
        break;
    };
  }

  void addVerticalGridLineTicksAuto(
      const juce::Rectangle<int>& bounds, const Plot::Scaling vertical_scaling,
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
      case Plot::Scaling::linear:
        addVerticalTicksLinear();
        break;
      case Plot::Scaling::logarithmic:
        addVerticalTicksLogarithmic();
        break;
      default:
        addVerticalTicksLinear();
        break;
    }
  }

  void addHorizontalGridLineTicksAuto(
      const juce::Rectangle<int>& bounds,
      const Plot::Scaling hotizontal_scaling, const bool tiny_grids,
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

      // Then create the horizontal lines
      auto y_diff = (y_lim.max - y_lim.min) / float(num_horizontal_lines);
      for (std::size_t i = 0u; i != num_horizontal_lines + 1u; ++i) {
        const auto y_pos = y_lim.min + float(i) * y_diff;
        y_ticks.push_back(y_pos);
      }
    };

    switch (hotizontal_scaling) {
      case Plot::Scaling::linear:
        addHorizontalTicksLinear();
        break;
      case Plot::Scaling::logarithmic:
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

  void createGridLabelsVertical(const juce::Rectangle<int>& bounds,
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

    auto m_custom_x_labels_reverse_it =
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
                            ? getNextCustomLabel(m_custom_x_labels_reverse_it)
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

  void createGridLabelsHorizontal(const juce::Rectangle<int>& bounds,
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

    auto m_custom_y_labels_reverse_it =
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
                            ? getNextCustomLabel(m_custom_y_labels_reverse_it)
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
};  // class PlotLookAndFeel
};  // namespace scp