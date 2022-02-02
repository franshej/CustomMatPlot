#include "cmp_lookandfeel.h"
#include "test_macros.h"

#define PI2 6.28318530718

TEST(random_y_values, real_time) {
  ADD_PLOT;
  ADD_TIMER(1000);

  GET_TIMER_CB = [=](const int dt_ms) {
    static std::vector<float> y_test_data(10);

    for (auto& y : y_test_data) {
      y = (float)rand() / RAND_MAX;
    }

    PLOT_Y({y_test_data});
  };
}

TEST(real_time_plot_function, real_time) {
  ADD_PLOT;
  ADD_TIMER(1000);

  X_LIM(1.f, 10.f);
  Y_LIM(0.f, 1.f);

  GET_TIMER_CB = [=](const int dt_ms) {
    static std::vector<float> y_test_data(10);

    for (auto& y : y_test_data) {
      y = (float)rand() / RAND_MAX;
    }

    REALTIMEPLOT({y_test_data});
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

  REALTIMEPLOT(getYData());

  const std::vector<cmp::GraphSpreadIndex> spread_indices = {{0, 1}};
  FILL_BETWEEN(spread_indices);

  GET_TIMER_CB = [=](const int dt_ms) { REALTIMEPLOT(getYData()); };
}
