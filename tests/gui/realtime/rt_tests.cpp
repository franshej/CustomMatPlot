/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "cmp_lookandfeel.h"
#include "test_macros.h"

#define PI2 6.28318530718

TEST(random_amount_y_data, real_time) {
  ADD_PLOT;
  ADD_TIMER(1000);

  GET_TIMER_CB = [=](const int dt_ms) {
    static cmp::SeriesDataList series;
    size_t points_count =
        2 + (size_t)((float(rand()) / float(RAND_MAX)) * 1000.0);
    size_t curve_count = 1 + (size_t)((float(rand()) / float(RAND_MAX)) * 10.0);

    series.resize(curve_count);
    for (size_t i = 0; i < curve_count; ++i) {
      series[i].y.resize(points_count);
      for (size_t j = 0; j < points_count; ++j) {
        series[i].y[j] =
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
      }
    }
    GET_PLOT->plot(series);
  };
}

TEST(random_y_values, real_time) {
  ADD_PLOT;
  ADD_TIMER(1000);

  GET_TIMER_CB = [=](const int dt_ms) {
    static std::vector<float> y_test_data(10);

    for (auto& y : y_test_data) {
      y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    }

    GET_PLOT->plot({.y = y_test_data});
  };
}

TEST(real_time_plot_function, real_time) {
  ADD_PLOT;
  ADD_TIMER(1000);

  X_LIM(1.f, 10.f);
  Y_LIM(0.f, 1.f);

  GET_PLOT->plot({.y = std::vector<float>(10)});
  GET_TIMER_CB = [=](const int dt_ms) {
    static std::vector<float> y_test_data(10);

    for (auto& y : y_test_data) {
      y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    }

    GET_PLOT->plotUpdateYOnly(y_test_data);
  };
}

TEST(spread, real_time) {
  ADD_PLOT;
  ADD_TIMER(10);

  auto getYData = []() {
    static std::vector<std::vector<float>> test_data_y =
        std::vector<std::vector<float>>(2, std::vector<float>(100));
    static auto t = 0.0f;

    float i = 0;
    for (auto& y_vec : test_data_y) {
      std::iota(y_vec.begin(), y_vec.end(), t);

      for (auto& y : y_vec) {
        y = i * std::sin(y * PI2 / float(y_vec.size()));
      }

      i += 10;
      t += 1.0f;
    }

    return test_data_y;
  };

  cmp::SeriesDataList series;
  for (const auto& y : getYData()) series.push_back({.y = y});
  GET_PLOT->plot(series);

  const std::vector<cmp::SpreadIndex> spread_indices = {{0, 1}};
  FILL_BETWEEN(spread_indices);

  GET_TIMER_CB = [=](const int dt_ms) {
    GET_PLOT->plotUpdateYOnly(getYData());
  };
}

TEST(random_yx_values, real_time) {
  ADD_PLOT;
  ADD_TIMER(1000);

  GET_TIMER_CB = [=](const int dt_ms) {
    static cmp::SeriesDataList series;
    size_t points_count =
        2 + (size_t)((float(rand()) / float(RAND_MAX)) * 1000.0);
    size_t curve_count = 1 + (size_t)((float(rand()) / float(RAND_MAX)) * 10.0);

    series.resize(curve_count);
    for (size_t i = 0; i < curve_count; ++i) {
      series[i].x.resize(points_count);
      series[i].y.resize(points_count);
      for (size_t j = 0; j < points_count; ++j) {
        series[i].x[j] = (float)j;  // Linear curve
        series[i].y[j] =
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
      }
    }
    GET_PLOT->plot(series);
  };
}
