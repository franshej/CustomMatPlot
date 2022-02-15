#include "cmp_lookandfeel.h"
#include "test_macros.h"

#define PI2 6.28318530718f

TEST(test_xy_ticks, non_real_time) {
  ADD_PLOT;

  std::vector<float> y_test_data(10);
  std::iota(y_test_data.begin(), y_test_data.end(), 1.f);

  std::vector<float> x_ticks = {0, 1, 7};
  std::vector<float> y_ticks = {3, 2, 9};

  PLOT_Y({y_test_data});
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

  PLOT_Y({y_test_data});
  X_LABELS(labels)
  X_LABEL("X LABEL");
  Y_LABEL("Y LABEL");
  TITLE("TITLE");
}

TEST(test_custom_y_labels, non_real_time) {
  ADD_PLOT;

  std::vector<float> y_test_data(10000);
  std::iota(y_test_data.begin(), y_test_data.end(), -100000.f);
  const std::vector<std::string> labels = {"Ett", "Two", "Three", "Fyra",
                                           "Fem", "Sex", "Sju",   "Atta",
                                           "Nio", "Tio", "Elva",  "Tolv"};

  PLOT_Y({y_test_data});
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

  PLOT_Y({y_test_data});
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

  PLOT_XY({y_test_data}, {x_test_data});
  GRID_ON;
}

TEST(test_grid_on, non_real_time) {
  ADD_PLOT;

  std::vector<float> y_test_data(10);
  std::iota(y_test_data.begin(), y_test_data.end(), 0);

  PLOT_Y({y_test_data});
  GRID_ON;
}

TEST(test_tiny_grid_on, non_real_time) {
  ADD_PLOT;

  std::vector<float> y_test_data(10);
  std::iota(y_test_data.begin(), y_test_data.end(), 0);

  PLOT_Y({y_test_data});
  TINY_GRID_ON;
}

TEST(test_semi_plot_x_tiny_grid_on, non_real_time) {
  ADD_SEMI_LOG_X;

  std::vector<float> y_test_data(1000);
  std::iota(y_test_data.begin(), y_test_data.end(), 1.f);
  PLOT_Y({y_test_data});
  TINY_GRID_ON;
};

TEST(test_draw_flat_line, non_real_time) {
  ADD_PLOT;

  std::vector<float> y_data{1, 1};
  std::vector<float> x_data{0, 9};

  PLOT_XY({y_data}, {x_data});
  Y_LIM(0, 2);
}

TEST(test_linear_dashed_lines, non_real_time) {
  ADD_PLOT;

  std::vector<float> y_test_data(10000);
  std::iota(y_test_data.begin(), y_test_data.end(), -100000.f);
  const std::vector<float> dashed_lengths = {4, 8};

  cmp::GraphAttribute ga;
  ga.dashed_lengths = dashed_lengths;

  PLOT_XY_ATTRI({y_test_data}, {}, {ga});
}

TEST(test_flat_curve_10000, non_real_time) {
  ADD_PLOT;

  std::vector<float> y_test_data(100000);
  std::iota(y_test_data.begin(), y_test_data.end(), -100000.f);

  PLOT_Y({y_test_data});
}

TEST(test_flat_curve_0p0001, non_real_time) {
  ADD_PLOT;

  std::vector<float> y_test_data(100);
  std::iota(y_test_data.begin(), y_test_data.end(), 0.f);
  for (auto &val : y_test_data) val *= 0.00001f;

  PLOT_Y({y_test_data});
}

TEST(test_labels, non_real_time) {
  ADD_PLOT;

  std::vector<float> y_test_data(10000);
  std::iota(y_test_data.begin(), y_test_data.end(), -100000.f);

  PLOT_Y({y_test_data});
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

  PLOT_Y({test_data});
}

TEST(test_ramp, non_real_time) {
  ADD_PLOT;

  std::vector<float> y_test_data(10);
  std::iota(y_test_data.begin(), y_test_data.end(), 0);

  PLOT_Y({y_test_data});
}

TEST(test_two_sine, non_real_time) {
  ADD_PLOT;

  std::vector<std::vector<float>> test_data_y =
      std::vector<std::vector<float>>(2, std::vector<float>(100));
  float i = 0;
  for (auto &y_vec : test_data_y) {
    std::iota(y_vec.begin(), y_vec.end(), 0);
    for (auto &y : y_vec) {
      y = std::sin(y * PI2 / y_vec.size()) + i;
    }
    i++;
  }

  std::vector<std::vector<float>> test_data_x =
      std::vector<std::vector<float>>(2, std::vector<float>(100));
  std::iota(test_data_x.front().begin(), test_data_x.front().end(), 0);
  std::iota(test_data_x.back().begin(), test_data_x.back().end(), -50);

  PLOT_XY(test_data_x, test_data_y);
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

  PLOT_XY({test_data_x}, {test_data_y});
  X_LIM(0, 50);
}

TEST(test_y_lim, non_real_time) {
  ADD_PLOT;

  std::vector<float> test_data_y = std::vector<float>(100);
  std::iota(test_data_y.begin(), test_data_y.end(), 0.f);
  for (auto &y : test_data_y) {
    y = std::sin(y * PI2 / test_data_y.size());
  }

  PLOT_Y({test_data_y});
  Y_LIM(0, 1);
}

TEST(test_legend_6, non_real_time) {
  ADD_PLOT;

  std::vector<std::vector<float>> test_data_y =
      std::vector<std::vector<float>>(6, std::vector<float>(100));
  std::vector<std::string> legends;

  float i = 0;
  for (auto &y_vec : test_data_y) {
    std::iota(y_vec.begin(), y_vec.end(), 0);
    for (auto &y : y_vec) {
      y = (i + 1) * std::sin(y * PI2 / y_vec.size()) + i;
    }
    std::stringstream stream;
    stream << std::fixed << std::setprecision(0) << (i + 1);
    legends.emplace_back("First sine with amplitude of: " + stream.str());
    i++;
  }

  PLOT_Y(test_data_y);
  LEGEND(legends);
}

TEST(test_legend_2, non_real_time) {
  ADD_PLOT;

  std::vector<std::vector<float>> test_data_y =
      std::vector<std::vector<float>>(2, std::vector<float>(100));
  std::vector<std::string> legends;

  float i = 0;
  for (auto &y_vec : test_data_y) {
    std::iota(y_vec.begin(), y_vec.end(), 0);
    for (auto &y : y_vec) {
      y = (i + 1) * std::sin(y * PI2 / y_vec.size()) + i;
    }
    std::stringstream stream;
    stream << std::fixed << std::setprecision(0) << (i + 1);
    legends.emplace_back("First sine with amplitude of: " + stream.str());
    i++;
  }

  PLOT_Y(test_data_y);
  LEGEND(legends);
}

TEST(plot_semi_log_y, non_real_time) {
  ADD_SEMI_LOG_Y;

  std::vector<float> y_test_data(1000);
  std::iota(y_test_data.begin(), y_test_data.end(), 1.f);

  PLOT_Y({y_test_data});
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
  PLOT_Y({y_test_data});
  GET_PLOT->setLookAndFeel(lnf.get());
}

TEST(set_custom_colour, non_real_time) {
  ADD_PLOT;

  std::vector<std::vector<float>> test_data_y =
      std::vector<std::vector<float>>(6, std::vector<float>(100));
  std::vector<std::string> legends;

  float i = 0;
  for (auto &y_vec : test_data_y) {
    std::iota(y_vec.begin(), y_vec.end(), 0);
    for (auto &y : y_vec) {
      y = (i + 1) * std::sin(y * PI2 / y_vec.size()) + i;
    }
    i++;
  }

  cmp::GraphAttributeList ga(6);
  ga[3].graph_colour = juce::Colours::pink;

  PLOT_XY_ATTRI(test_data_y, {}, ga);
  LEGEND(legends);
}

TEST(opacity, non_real_time) {
  ADD_SEMI_LOG_Y;

  std::vector<float> y_test_data(1000);
  std::iota(y_test_data.begin(), y_test_data.end(), 1.f);

  cmp::GraphAttributeList ga(1);
  ga[0].graph_line_opacity = 0.5f;

  PLOT_XY_ATTRI({y_test_data}, {}, ga);
  GRID_ON;
}

TEST(path_stroke_path, non_real_time) {
  ADD_PLOT;

  std::vector<float> y_test_data(1000);
  std::iota(y_test_data.begin(), y_test_data.end(), 1.f);

  cmp::GraphAttributeList ga(1);
  ga[0].path_stroke_type = juce::PathStrokeType(10);

  PLOT_XY_ATTRI({y_test_data}, {}, ga);
  GRID_ON;
}

TEST(markers, non_real_time) {
  ADD_PLOT;

  std::vector<std::vector<float>> test_data_y =
      std::vector<std::vector<float>>(7, std::vector<float>(10));
  std::vector<std::string> legends;

  float i = 0;
  for (auto &y_vec : test_data_y) {
    std::iota(y_vec.begin(), y_vec.end(), 0);
    for (auto &y : y_vec) {
      y = (i + 1) * std::sin(y * PI2 / y_vec.size()) + i;
    }
    ++i;
  }
  cmp::GraphAttributeList ga(test_data_y.size());
  ga[0].marker= cmp::Marker::Type::Circle;

  ga[1].marker = cmp::Marker::Type::Square;
  ga[1].marker.value().FaceColour = juce::Colours::lightpink;

  ga[2].marker = cmp::Marker::Type::Pentagram;

  ga[3].marker = cmp::Marker::Type::UpTriangle;

  ga[4].marker = cmp::Marker::Type::RightTriangle;

  ga[5].marker = cmp::Marker::Type::DownTriangle;

  ga[6].marker = cmp::Marker::Type::LeftTriangle;

  PLOT_XY_ATTRI({test_data_y}, {}, ga);
  GRID_ON;
}

TEST(spread, non_real_time) {
  ADD_PLOT;

  std::vector<std::vector<float>> test_data_y =
      std::vector<std::vector<float>>(4, std::vector<float>(10));

  float i = 0;
  for (auto &y_vec : test_data_y) {
    std::iota(y_vec.begin(), y_vec.end(), 0);
    for (auto &y : y_vec) {
      y = i * std::sin(y * PI2 / y_vec.size());
    }
    i += 10;
  }

  const std::vector<cmp::GraphSpreadIndex> spread_indices = {{0, 1}, {2, 3}};

  PLOT_Y(test_data_y);
  FILL_BETWEEN(spread_indices);
}