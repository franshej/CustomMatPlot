#include "scp_lookandfeel.h"
#include "test_macros.h"

#define PI2 6.28318530718

TEST(test_xy_ticks, RealTime) {
  ADD_PLOT;
  ADD_TIMER(1000);
  parent_component->getBounds();

  GET_TIMER_CB = [=](const int dt_ms) {
    static std::vector<float> y_test_data(10);
    std::iota(y_test_data.begin(), y_test_data.end(), 1.f);

    for (auto &y : y_test_data) {
      y = (float)rand() / RAND_MAX;
    }

    PLOT_Y({y_test_data});
  };
}