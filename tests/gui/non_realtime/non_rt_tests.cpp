/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <cmath>
#include <vector>

#include "cmp_datamodels.h"
#include "cmp_lookandfeel.h"
#include "test_macros.h"

#define PI2 6.28318530718f

TEST(test_xy_ticks, non_real_time) {
  ADD_PLOT;

  std::vector<float> y_test_data(10);
  std::iota(y_test_data.begin(), y_test_data.end(), 1.f);

  std::vector<float> x_ticks = {0, 1, 7};
  std::vector<float> y_ticks = {3, 2, 9};

  GET_PLOT->plot({.y = y_test_data});
  X_TICKS(x_ticks);
  Y_TICKS(y_ticks);
}

TEST(test_custom_x_labels, non_real_time) {
  ADD_PLOT;

  std::vector<float> y_test_data(10000);
  std::iota(y_test_data.begin(), y_test_data.end(), -100000.f);
  const std::vector<std::string> labels = {"MMM", "Two", "Three", "Fyra",
                                           "Fem", "Sex", "Sju",   "Atta",
                                           "Nio", "Tio", "Elva",  "Tolv"};

  GET_PLOT->plot({.y = y_test_data});
  X_LABELS(labels)
  X_LABEL("X LABEL");
  Y_LABEL("Y LABEL");
  TITLE("TITLE");
}

TEST(test_custom_x_labels_ticks, non_real_time) {
  ADD_PLOT;

  std::vector<float> y_test_data(10000);
  std::iota(y_test_data.begin(), y_test_data.end(), -100000.f);
  const std::vector<std::string> labels = {"MMM", "Two", "Three", "Fyra"};
  std::vector<float> x_ticks = {0, 3000, 7000, 10000};

  GET_PLOT->plot({.y = y_test_data});
  X_LABELS(labels)
  X_TICKS(x_ticks);
  X_LABEL("X LABEL");
  Y_LABEL("Y LABEL");
  TITLE("TITLE");
}

TEST(test_custom_y_labels, non_real_time) {
  ADD_PLOT;

  std::vector<float> y_test_data(10000);
  std::iota(y_test_data.begin(), y_test_data.end(), -100000.f);
  static const std::vector<std::string> labels = {
      "Ett",     "Two",     "Three",  "Fyra",   "Fem",    "Sex",
      "Sju",     "Atta",    "Nio",    "Tio",    "Elva",   "Tolv",
      "Tretton", "Fjorton", "Femton", "Sexton", "sjutton"};

  GET_PLOT->plot({.y = y_test_data});
  Y_LABELS(labels)
  X_LABEL("X LABEL");
  Y_LABEL("Y LABEL");
  TITLE("TITLE");
}

TEST(test_semi_log_x_1000, non_real_time) {
  ADD_SEMI_LOG_X;

  std::vector<float> y_test_data(1000);
  std::iota(y_test_data.begin(), y_test_data.end(), 0.f);

  y_test_data[9] = 1000;
  y_test_data[99] = 1000;

  GET_PLOT->plot({.y = y_test_data});
  GRID_ON;
}

TEST(test_semi_log_xy_1000, non_real_time) {
  ADD_SEMI_LOG_X;

  std::vector<float> y_test_data(1000);
  std::iota(y_test_data.begin(), y_test_data.end(), 1.f);

  y_test_data[9] = 1000;
  y_test_data[99] = 1000;

  std::vector<float> x_test_data(1000);
  std::iota(x_test_data.begin(), x_test_data.end(), 1.f);
  for (auto &x : x_test_data) x = x * 1e-3f;

  GET_PLOT->plot({.x = x_test_data, .y = y_test_data});
  GRID_ON;
}

TEST(test_grid_on, non_real_time) {
  ADD_PLOT;

  std::vector<float> y_test_data(10);
  std::iota(y_test_data.begin(), y_test_data.end(), 0.f);

  GET_PLOT->plot({.y = y_test_data});
  GRID_ON;
}

TEST(test_tiny_grid_on, non_real_time) {
  ADD_PLOT;

  std::vector<float> y_test_data(10);
  std::iota(y_test_data.begin(), y_test_data.end(), 0.0f);

  GET_PLOT->plot({.y = y_test_data});
  TINY_GRID_ON;
}

TEST(test_semi_plot_x_tiny_grid_on, non_real_time) {
  ADD_SEMI_LOG_X;

  std::vector<float> y_test_data(1000);
  std::iota(y_test_data.begin(), y_test_data.end(), 1.f);
  GET_PLOT->plot({.y = y_test_data});
  TINY_GRID_ON;
};

TEST(test_draw_flat_line, non_real_time) {
  ADD_PLOT;

  std::vector<float> y_data{1, 1};
  std::vector<float> x_data{0, 9};

  GET_PLOT->plot({.x = x_data, .y = y_data});
  Y_LIM(0, 2);
}

TEST(test_linear_dashed_lines, non_real_time) {
  ADD_PLOT;

  std::vector<float> y_test_data(10000);
  std::iota(y_test_data.begin(), y_test_data.end(), -100000.f);
  const std::vector<float> dashed_lengths = {4, 8};

  cmp::SeriesAttribute ga;
  ga.dashed_lengths = dashed_lengths;

  GET_PLOT->plot({.y = y_test_data, .attribute = ga});
}

TEST(test_flat_curve_10000, non_real_time) {
  ADD_PLOT;

  std::vector<float> y_test_data(100000);
  std::iota(y_test_data.begin(), y_test_data.end(), -100000.f);

  GET_PLOT->plot({.y = y_test_data});
}

TEST(test_flat_curve_0p0001, non_real_time) {
  ADD_PLOT;

  std::vector<float> y_test_data(100);
  std::iota(y_test_data.begin(), y_test_data.end(), 0.f);
  for (auto &val : y_test_data) val *= 0.00001f;

  GET_PLOT->plot({.y = y_test_data});
}

TEST(test_labels, non_real_time) {
  ADD_PLOT;

  std::vector<float> y_test_data(10000);
  std::iota(y_test_data.begin(), y_test_data.end(), -100000.f);

  GET_PLOT->plot({.y = y_test_data});
  X_LABEL("X LABEL");
  Y_LABEL("Y LABEL");
  TITLE("TITLE");
}

TEST(test_sinus_auto_lim, non_real_time) {
  ADD_PLOT;

  std::vector<float> test_data = std::vector<float>(100);
  std::iota(test_data.begin(), test_data.end(), 0.f);
  for (auto &y : test_data) {
    y = std::sin(y * PI2 / test_data.size());
  }

  GET_PLOT->plot({.y = test_data});
}

TEST(test_ramp, non_real_time) {
  ADD_PLOT;

  std::vector<float> y_test_data(10);
  std::iota(y_test_data.begin(), y_test_data.end(), 0);

  GET_PLOT->plot({.y = y_test_data});
}

TEST(test_two_sine, non_real_time) {
  ADD_PLOT;

  cmp::SeriesDataList series(2);

  // x-values: two sine waves.
  float i = 0;
  for (auto &s : series) {
    s.x.resize(100);
    std::iota(s.x.begin(), s.x.end(), 0);
    for (auto &x : s.x) x = std::sin(x * PI2 / s.x.size()) + i;
    i++;
  }

  // y-values: two ramps.
  series[0].y.resize(100);
  std::iota(series[0].y.begin(), series[0].y.end(), 0);
  series[1].y.resize(100);
  std::iota(series[1].y.begin(), series[1].y.end(), -50);

  GET_PLOT->plot(series);
}

TEST(test_x_lim, non_real_time) {
  ADD_PLOT;

  std::vector<float> test_data_y = std::vector<float>(100);
  std::iota(test_data_y.begin(), test_data_y.end(), 0.f);
  for (auto &y : test_data_y) {
    y = std::sin(y * PI2 / test_data_y.size());
  }

  std::vector<float> test_data_x = std::vector<float>(100);
  std::iota(test_data_x.begin(), test_data_x.end(), -49);

  GET_PLOT->plot({.x = test_data_y, .y = test_data_x});
  X_LIM(0, 50);
}

TEST(test_y_lim, non_real_time) {
  ADD_PLOT;

  std::vector<float> test_data_y = std::vector<float>(100);
  std::iota(test_data_y.begin(), test_data_y.end(), 0.f);
  for (auto &y : test_data_y) {
    y = std::sin(y * PI2 / test_data_y.size());
  }

  GET_PLOT->plot({.y = test_data_y});
  Y_LIM(0, 1);
}

TEST(test_legend_6, non_real_time) {
  ADD_PLOT;

  cmp::SeriesDataList series(6);
  std::vector<std::string> legends;

  float i = 0;
  for (auto &s : series) {
    s.y.resize(100);
    std::iota(s.y.begin(), s.y.end(), 0);
    for (auto &y : s.y) {
      y = (i + 1) * std::sin(y * PI2 / s.y.size()) + i;
    }

    std::stringstream stream;
    stream << std::fixed << std::setprecision(0) << (i + 1);
    legends.emplace_back("First sine with amplitude of: " + stream.str());
    i++;
  }

  GET_PLOT->plot(series);
  LEGEND(legends);

  TITLE("Some good looking sines!")
  X_LABEL("Time (s)")
  Y_LABEL("Voltage (V)")

  GRID_ON;
}

TEST(test_legend_2, non_real_time) {
  ADD_PLOT;

  cmp::SeriesDataList series(2);
  std::vector<std::string> legends;

  float i = 0;
  for (auto &s : series) {
    s.y.resize(100);
    std::iota(s.y.begin(), s.y.end(), 0);
    for (auto &y : s.y) {
      y = (i + 1) * std::sin(y * PI2 / s.y.size()) + i;
    }
    std::stringstream stream;
    stream << std::fixed << std::setprecision(0) << (i + 1);
    legends.emplace_back("First sine with amplitude of: " + stream.str());
    i++;
  }

  GET_PLOT->plot(series);
  LEGEND(legends);
}

TEST(plot_semi_log_y, non_real_time) {
  ADD_SEMI_LOG_Y;

  std::vector<float> y_test_data(1000);
  std::iota(y_test_data.begin(), y_test_data.end(), 1.f);

  GET_PLOT->plot({.y = y_test_data});
  GRID_ON;
}

class MyLnf : public cmp::PlotLookAndFeel {
  juce::Font getGridLabelFont() const noexcept override {
    return juce::Font("Arial Rounded MT", 32.f, juce::Font::plain);
  };
};

TEST(look_and_feel, non_real_time) {
  ADD_SEMI_LOG_Y;

  std::vector<float> y_test_data(1000);
  std::iota(y_test_data.begin(), y_test_data.end(), 1.f);
  auto lnf = std::make_shared<MyLnf>();

  PARENT->lnf.emplace_back(lnf);
  GET_PLOT->plot({.y = y_test_data});
  GET_PLOT->setLookAndFeel(lnf.get());
}

class MyLnf_2 : public cmp::PlotLookAndFeel {
  bool isXAxisLabelsBelowAxesArea() const noexcept override { return false; }
};

TEST(x_axis_above_axes, non_real_time) {
  ADD_SEMI_LOG_Y;

  std::vector<float> y_test_data(1000);
  std::iota(y_test_data.begin(), y_test_data.end(), 1.f);
  auto lnf = std::make_shared<MyLnf_2>();

  PARENT->lnf.emplace_back(lnf);
  GET_PLOT->plot({.y = y_test_data});
  GET_PLOT->setLookAndFeel(lnf.get());
}

TEST(set_custom_colour, non_real_time) {
  ADD_PLOT;

  cmp::SeriesDataList series(6);
  std::vector<std::string> legends;

  float i = 0;
  for (auto &s : series) {
    s.y.resize(100);
    std::iota(s.y.begin(), s.y.end(), 0);
    for (auto &y : s.y) {
      y = (i + 1) * std::sin(y * PI2 / s.y.size()) + i;
    }
    i++;
  }

  series[3].attribute.series_colour = juce::Colours::pink;

  GET_PLOT->plot(series);
  LEGEND(legends);
}

TEST(opacity, non_real_time) {
  ADD_SEMI_LOG_Y;

  std::vector<float> y_test_data(1000);
  std::iota(y_test_data.begin(), y_test_data.end(), 1.f);

  cmp::SeriesAttributeList ga(1);
  ga[0].series_opacity = 0.5f;

  GET_PLOT->plot({.y = y_test_data, .attribute = ga[0]});
  GRID_ON;
}

TEST(path_stroke_path, non_real_time) {
  ADD_PLOT;

  std::vector<float> y_test_data(1000);
  std::iota(y_test_data.begin(), y_test_data.end(), 1.f);

  cmp::SeriesAttributeList ga(1);
  ga[0].path_stroke_type = juce::PathStrokeType(10);

  GET_PLOT->plot({.y = y_test_data, .attribute = ga[0]});
  GRID_ON;
}

TEST(markers, non_real_time) {
  ADD_PLOT;

  cmp::SeriesDataList series(7);
  std::vector<std::string> legends;

  float i = 0;
  for (auto &s : series) {
    s.y.resize(10);
    std::iota(s.y.begin(), s.y.end(), 0);
    for (auto &y : s.y) {
      y = (i + 1) * std::sin(y * PI2 / s.y.size()) + i;
    }
    ++i;
  }
  series[0].attribute.marker = cmp::Marker::Type::Circle;

  series[1].attribute.marker = cmp::Marker::Type::Square;
  series[1].attribute.marker.value().FaceColour = juce::Colours::lightpink;

  series[2].attribute.marker = cmp::Marker::Type::Pentagram;

  series[3].attribute.marker = cmp::Marker::Type::UpTriangle;

  series[4].attribute.marker = cmp::Marker::Type::RightTriangle;

  series[5].attribute.marker = cmp::Marker::Type::DownTriangle;

  series[6].attribute.marker = cmp::Marker::Type::LeftTriangle;

  GET_PLOT->plot(series);
  GRID_ON;
}

TEST(spread, non_real_time) {
  ADD_SEMI_LOG_X;

  cmp::SeriesDataList series(4);

  float i = 0;
  for (auto &s : series) {
    s.y.resize(1000);
    std::iota(s.y.begin(), s.y.end(), 0);
    for (auto &y : s.y) {
      y = i * std::sin(y * PI2 / s.y.size());
    }
    i += 10;
  }

  const std::vector<cmp::SpreadIndex> spread_indices = {{0, 1}, {2, 3}};

  GET_PLOT->plot(series);
  FILL_BETWEEN(spread_indices);

  X_LABEL("Logarithmic x-axis");
  Y_LABEL("Linear y-axis");
  TITLE("Nice looking fills between series!");
}

TEST(xy_downsampling, non_real_time) {
  ADD_PLOT;

  std::vector<float> y_data(1'000'000u, 0.0f);

  y_data[500'000u] = 1.0f;
  y_data[500'004u] = -31.0f;
  y_data[500'006u] = 14.0f;

  GET_PLOT->plot({.y = y_data});
}

TEST(set_trace_point, non_real_time) {
  ADD_PLOT;

  std::vector<float> y_data_1(1'000u, 0.0f);
  std::iota(y_data_1.begin(), y_data_1.end(), 0.0f);

  std::vector<float> y_data_2(1'000u, 0.0f);
  std::iota(y_data_2.begin(), y_data_2.end(), -100.0f);

  GET_PLOT->plot({{.y = y_data_1}, {.y = y_data_2}});

  SET_TRACE_POINT(juce::Point<float>(100.0f, 100.0f));
}

TEST(clear_all_tracepoints, non_real_time) {
  ADD_PLOT;

  std::vector<float> y_data_1(1'000u, 0.0f);
  std::iota(y_data_1.begin(), y_data_1.end(), 0.0f);

  std::vector<float> y_data_2(1'000u, 0.0f);
  std::iota(y_data_2.begin(), y_data_2.end(), -100.0f);

  GET_PLOT->plot({{.y = y_data_1}, {.y = y_data_2}});

  SET_TRACE_POINT(juce::Point<float>(100.0f, 100.0f));

  CLEAR_TRACE_POINTS
}

TEST(set_scaling_dynamic, non_real_time) {
  ADD_PLOT;

  std::vector<float> y_data_1(1'000u, 0.0f);
  std::iota(y_data_1.begin(), y_data_1.end(), 1.0f);

  std::vector<float> y_data_2(1'000u, 0.0f);
  std::iota(y_data_2.begin(), y_data_2.end(), 100.0f);

  GET_PLOT->plot({{.y = y_data_1}, {.y = y_data_2}});

  SET_TRACE_POINT(juce::Point<float>(100.0f, 100.0f));

  Y_LIM(100, 1100);
  X_LIM(1, 1000);

  SET_SCALING(cmp::Scaling::logarithmic, cmp::Scaling::logarithmic);
}

TEST(step_function_downsample, non_real_time) {
  ADD_PLOT;

  std::vector<float> y(10u, 0.0f);
  y[4] = 1.0f;
  y[5] = 1.0f;

  std::vector<float> x(10u);
  std::iota(x.begin(), x.end(), 0.0f);

  x[3] = 4.f;
  x[6] = 5.f;

  GET_PLOT->plot({.x = x, .y = y});
}

TEST(trace_point_cb, non_real_time) {
  ADD_SEMI_LOG_X;

  std::vector<float> y_data_1(1'000u, 0.0f);
  std::iota(y_data_1.begin(), y_data_1.end(), 0.0f);

  std::vector<float> y_data_2(1'000u, 0.0f);
  std::iota(y_data_2.begin(), y_data_2.end(), -100.0f);

  GET_PLOT->plot({{.y = y_data_1}, {.y = y_data_2}});

  SET_TRACE_POINT(juce::Point<float>(100.0f, 100.0f));

  GET_PLOT->onTraceValueChange = [](auto comp, auto prev_data, auto new_data) {
    DBG("Prev X: " + std::to_string(prev_data.getX()) +
        "Prev Y: " + std::to_string(prev_data.getY()) +
        "\nNew X: " + std::to_string(new_data.getX()) +
        "New Y: " + std::to_string(new_data.getY()));
  };
}

template <typename T>
static auto create_heart(const T amplitude, const std::size_t N) {
  auto in_type = amplitude;

  std::vector<decltype(in_type)> x(N), y(N);

  for (auto i = 0; i < N; ++i) {
    const auto t = 2 * juce::MathConstants<decltype(in_type)>::pi * i / N;

    constexpr auto k = (13.0f / 16.0f);
    constexpr auto k1 = (5.0f / 16.0f);
    constexpr auto k2 = (2.0f / 16.0f);
    constexpr auto k3 = (1.0f / 16.0f);

    x.at(i) = amplitude * sin(t) * sin(t) * sin(t);
    y.at(i) = amplitude * (k * cos(t) - k1 * cos(2 * t) - k2 * cos(3 * t) -
                           k3 * cos(4 * t));
  }

  return std::make_pair(x, y);
}

TEST(heart, non_real_time) {
  ADD_PLOT;

  constexpr auto num_hearts = 10;
  cmp::SeriesDataList series(num_hearts);

  for (size_t i = 0; i < num_hearts; ++i) {
    const auto [x, y] = create_heart(float(i), 5000u);

    series[i].x = x;
    series[i].y = y;
  }

  GET_PLOT->plot(series);
}

TEST(test_vertical_line, non_real_time) {
  ADD_PLOT;
  { GET_PLOT->plotVerticalLines({7.0f}); }

  {
    std::vector<float> y_test_data(10);
    std::iota(y_test_data.begin(), y_test_data.end(), 0);
    GET_PLOT->plot({.y = y_test_data});
  }

  { GET_PLOT->plotVerticalLines({3.0f, 10.0f, 5.0f}); }
}

TEST(test_horizontal_line, non_real_time) {
  ADD_PLOT;
  { GET_PLOT->plotHorizontalLines({5.0f}); }

  {
    std::vector<float> y_test_data(10);
    std::iota(y_test_data.begin(), y_test_data.end(), 0);
    GET_PLOT->plot({.y = y_test_data});
  }

  { GET_PLOT->plotHorizontalLines({3.0f, 10.0f, 5.0f}); }
}

TEST(gradient, non_real_time) {
  ADD_PLOT;

  std::vector<float> y_test_data(1000);
  std::iota(y_test_data.begin(), y_test_data.end(), 1.f);

  cmp::SeriesAttributeList ga(1);
  juce::Colour colour0 = juce::Colours::red;
  juce::Colour colour1 = juce::Colours::green;
  ga[0].gradient_colours = std::make_pair(colour0, colour1);

  GET_PLOT->plot({.y = y_test_data, .attribute = ga[0]});
  GRID_ON;
}