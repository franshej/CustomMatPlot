#include "scp_lookandfeel.h"
#include "test_macros.h"

#define PI2 6.28318530718

TEST(random_y_values, RealTime) {
  ADD_PLOT;
  ADD_TIMER(1000);
  parent_component->getBounds();

  GET_TIMER_CB = [=](const int dt_ms) {
    static std::vector<float> y_test_data(10);

    for (auto& y : y_test_data) {
      y = (float)rand() / RAND_MAX;
    }

    PLOT_Y({y_test_data});
  };
}

TEST(real_time_plot_function, RealTime) {
  ADD_PLOT;
  ADD_TIMER(100);
  parent_component->getBounds();

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
