#pragma once

#include "spl_plot.h"

namespace scp {
class PlotLookAndFeel : public juce::LookAndFeel_V3,
                        public Plot::LookAndFeelMethods {
 public:
  PlotLookAndFeel() { setDefaultPlotColours(); }

  virtual ~PlotLookAndFeel() override{};

  void setDefaultPlotColours() noexcept override {
    setColour(scp::Plot::background_colour, juce::Colour(0xff566573));
    setColour(scp::Plot::frame_colour, juce::Colour(0xffcacfd2));

    setColour(scp::Plot::grid_colour, juce::Colour(0xff99A3A4));
    setColour(scp::Plot::x_grid_label_colour, juce::Colour(0xffaab7b8));
    setColour(scp::Plot::y_grid_label_colour, juce::Colour(0xffaab7b8));

    setColour(scp::Plot::title_label_colour, juce::Colour(0xffecf0f1));
    setColour(scp::Plot::x_label_colour, juce::Colour(0xffecf0f1));
    setColour(scp::Plot::y_label_colour, juce::Colour(0xffecf0f1));

    setColour(scp::Plot::first_graph_colour, juce::Colour(0xffec7063));
    setColour(scp::Plot::second_graph_colour, juce::Colour(0xffa569Bd));
    setColour(scp::Plot::third_graph_colour, juce::Colour(0xff85c1e9));
    setColour(scp::Plot::fourth_graph_colour, juce::Colour(0xff73c6b6));
    setColour(scp::Plot::fifth_graph_colour, juce::Colour(0xfff4d03f));
    setColour(scp::Plot::sixth_graph_colour, juce::Colour(0xffeB984e));
  }

  juce::Rectangle<int> getPlotBounds(
      juce::Rectangle<int>& bounds) const override {
    return juce::Rectangle<int>(0, 0, bounds.getWidth(), bounds.getHeight());
  }

  juce::Rectangle<int> getGraphBounds(
      juce::Rectangle<int>& bounds) const override {
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
        case scp::Plot::GraphType::GraphLine:
          g.setColour(findColour(getColourFromGraphID(graph_id)));
          break;
        case scp::Plot::GraphType::GridLine:
          g.setColour(findColour(scp::Plot::ColourIds::grid_colour));
          break;
        default:
          g.setColour(juce::Colours::pink);
          break;
      }

      g.strokePath(graph_path, p_type);
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
        if (x < x_lim.max) {
          graph_points[i].setX(xToXPos(x));
          i++;
        }
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

  void addVerticalGridLineTicksAuto(const juce::Rectangle<int>& bounds,
                                    const Plot::Scaling vertical_scaling,
                                    std::vector<float>& x_ticks,
                                    Lim_f x_lim) noexcept override {
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

  void addHorizontalGridLineTicksAuto(const juce::Rectangle<int>& bounds,
                                      const Plot::Scaling hotizontal_scaling,
                                      std::vector<float>& y_ticks,
                                      Lim_f y_lim) noexcept override {
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

};  // class PlotLookAndFeel
};  // namespace scp