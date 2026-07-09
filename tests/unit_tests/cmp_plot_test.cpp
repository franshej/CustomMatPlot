#include "cmp_plot.h"

#include <juce_core/juce_core.h>

#include <memory>

#include "cmp_datamodels.h"
#include "cmp_lookandfeel.h"
#include "cmp_series.h"
#include "cmp_test_helper.hpp"

SECTION(PlotClass, "Plot class") {
  auto expectEqualsLambda = [&](auto a, auto b) { expectEquals(a, b); };
  const std::vector<float> x_data1 = {1.f, 2.f};
  const std::vector<float> y_data1 = {100.f, 200.f};
  const std::vector<float> x_data2 = {1.f, 2.f, 3.f, 4.f};
  const std::vector<float> y_data2 = {200.f, 300.f, 400.f, 500.f};
  const std::vector<float> x_data3 = {1.f, 2.f};
  const std::vector<float> y_data3 = {400.f, 500.f};
  const std::vector<float> x_data_random_1 = {32.f, 45.f};
  const std::vector<float> x_data_random_2 = {432156.f, 43.f, 2123.f, 553.f};
  const std::vector<float> x_data_random_3 = {5321.f, 4215.f};
  cmp::Plot plot;

  TEST("Empty series.") {
    auto series = getChildComponentHelper<cmp::Series>(plot);
    expect(series.empty());
  }

  TEST("Single series") {
    plot.plot({y_data1});
    const auto series = getChildComponentHelper<cmp::Series>(plot);
    expectEquals(series.size(), 1ul);
    const auto& x_data = series[0]->getXData();
    const auto& y_data = series[0]->getYData();
    expectEqualVectors(x_data, x_data1, expectEqualsLambda);
    expectEqualVectors(y_data, y_data1, expectEqualsLambda);
  }

  TEST("Several series") {
    plot.plot({y_data1, y_data2, y_data3});
    auto series = getChildComponentHelper<cmp::Series>(plot);
    expectEquals(series.size(), 3ul);
    expectEqualVectors(series[0]->getXData(), x_data1, expectEqualsLambda);
    expectEqualVectors(series[0]->getYData(), y_data1, expectEqualsLambda);
    expectEqualVectors(series[1]->getXData(), x_data2, expectEqualsLambda);
    expectEqualVectors(series[1]->getYData(), y_data2, expectEqualsLambda);
    expectEqualVectors(series[2]->getXData(), x_data3, expectEqualsLambda);
    expectEqualVectors(series[2]->getYData(), y_data3, expectEqualsLambda);
  }

  TEST("Horizontal line") {
    cmp::Plot horizontal_plot;
    horizontal_plot.plotHorizontalLines({100.f});
    auto series = getChildComponentHelper<cmp::Series>(horizontal_plot);
    expectEquals(series.size(), 1ul);
    expect(series[0]->getType() == cmp::SeriesType::horizontal);
    expectEqualVectors(series[0]->getYData(), {100.f, 100.f},
                       expectEqualsLambda);
  }

  TEST("Vertical line") {
    cmp::Plot vertical_plot;
    vertical_plot.plotVerticalLines({100.f});
    auto series = getChildComponentHelper<cmp::Series>(vertical_plot);
    expectEquals(series.size(), 1ul);
    expect(series[0]->getType() == cmp::SeriesType::vertical);
    expectEqualVectors(series[0]->getXData(), {100.f, 100.f},
                       expectEqualsLambda);
  }

  TEST("Update Y data only") {
    plot.plot({y_data1, y_data2, y_data3},
              {x_data_random_1, x_data_random_2, x_data_random_3});
    plot.plotUpdateYOnly({y_data1, y_data2, y_data3});
    auto series = getChildComponentHelper<cmp::Series>(plot);
    expectEquals(series.size(), 3ul);
    expectEqualVectors(series[0]->getXData(), x_data_random_1,
                       expectEqualsLambda);
    expectEqualVectors(series[0]->getYData(), y_data1, expectEqualsLambda);
    expectEqualVectors(series[1]->getXData(), x_data_random_2,
                       expectEqualsLambda);
    expectEqualVectors(series[1]->getYData(), y_data2, expectEqualsLambda);
    expectEqualVectors(series[2]->getXData(), x_data_random_3,
                       expectEqualsLambda);
    expectEqualVectors(series[2]->getYData(), y_data3, expectEqualsLambda);
  }

  TEST("Set colour") {
    cmp::Plot plot_tmp;
    plot_tmp.getLookAndFeel().setColour(cmp::Plot::grid_colour,
                                        juce::Colours::red);
    const auto red =
        plot_tmp.getLookAndFeel().findColour(cmp::Plot::grid_colour);
    expect(red == juce::Colours::red);
  }

  TEST("Custom look and feel") {
    cmp::Plot plot_tmp;
    std::unique_ptr<cmp::PlotLookAndFeel> look_and_feel =
        std::make_unique<cmp::PlotLookAndFeel>();
    plot_tmp.setLookAndFeel(look_and_feel.get());
    expect(&plot_tmp.getLookAndFeel() == look_and_feel.get());
    plot_tmp.setLookAndFeel(nullptr);
  }
}
