#include <numeric>
#include <vector>

#include "cmp_lookandfeel.h"
#include "cmp_series.h"
#include "cmp_test_helper.hpp"
#include "cmp_utils.h"

/* Characterization tests for the pixel-point pipeline.
 *
 * These tests pin down the current behaviour of
 * PlotLookAndFeel::updateXPixelPoints/updateYPixelPoints and the full
 * data -> pixel-point pipeline in cmp::Plot/Series before the pipeline is
 * routed through a shared per-axis transform (see docs/3d-plan.md, Phase 0).
 *
 * Conventions pinned down here:
 * - Pixel points are produced in axes-area-local coordinates (only the
 *   width/height of the axes bounds are used).
 * - The caller owns the sizing of the pixel-point vector;
 *   updateXPixelPoints/updateYPixelPoints only write coordinates, so they
 *   can run in any order.
 */
SECTION(PixelPointsPipelineTest, "Pixel points pipeline") {
  auto expectNear = [&](const float result, const float expected) {
    expectWithinAbsoluteError(result, expected,
                              1e-3f * (1.0f + std::abs(expected)));
  };

  const auto axes_bounds = juce::Rectangle<int>(0, 0, 500, 400);

  auto makeIotaIndices = [](const std::size_t size) {
    std::vector<std::size_t> indices(size);
    std::iota(indices.begin(), indices.end(), 0u);
    return indices;
  };

  TEST("updateXPixelPoints: linear") {
    cmp::PlotLookAndFeel lnf;
    const std::vector<float> x_data = {0.f, 2.5f, 5.f, 7.5f, 10.f};
    auto indices = makeIotaIndices(x_data.size());
    cmp::PixelPoints pixel_points(indices.size());

    lnf.updateXPixelPoints({}, cmp::Scaling::linear, {0.f, 10.f}, axes_bounds,
                           x_data, indices, pixel_points);

    const std::vector<float> expected = {0.f, 125.f, 250.f, 375.f, 500.f};
    expectEquals(pixel_points.size(), x_data.size());
    for (std::size_t i = 0; i < expected.size(); ++i) {
      expectNear(pixel_points[i].getX(), expected[i]);
    }
  }

  TEST("updateYPixelPoints: linear (y-axis is inverted)") {
    cmp::PlotLookAndFeel lnf;
    const std::vector<float> x_data = {0.f, 5.f, 10.f};
    const std::vector<float> y_data = {0.f, 5.f, 10.f};
    auto indices = makeIotaIndices(y_data.size());
    cmp::PixelPoints pixel_points(indices.size());

    lnf.updateXPixelPoints({}, cmp::Scaling::linear, {0.f, 10.f}, axes_bounds,
                           x_data, indices, pixel_points);
    lnf.updateYPixelPoints({}, cmp::Scaling::linear, {0.f, 10.f}, axes_bounds,
                           y_data, indices, pixel_points);

    const std::vector<float> expected = {400.f, 200.f, 0.f};
    expectEquals(pixel_points.size(), y_data.size());
    for (std::size_t i = 0; i < expected.size(); ++i) {
      expectNear(pixel_points[i].getY(), expected[i]);
    }
  }

  TEST("updateX/YPixelPoints: logarithmic") {
    cmp::PlotLookAndFeel lnf;
    const auto log_bounds = juce::Rectangle<int>(0, 0, 300, 300);
    const std::vector<float> data = {1.f, 10.f, 100.f, 1000.f};
    auto indices = makeIotaIndices(data.size());
    cmp::PixelPoints pixel_points(indices.size());

    lnf.updateXPixelPoints({}, cmp::Scaling::logarithmic, {1.f, 1000.f},
                           log_bounds, data, indices, pixel_points);
    lnf.updateYPixelPoints({}, cmp::Scaling::logarithmic, {1.f, 1000.f},
                           log_bounds, data, indices, pixel_points);

    const std::vector<float> expected_x = {0.f, 100.f, 200.f, 300.f};
    const std::vector<float> expected_y = {300.f, 200.f, 100.f, 0.f};
    expectEquals(pixel_points.size(), data.size());
    for (std::size_t i = 0; i < data.size(); ++i) {
      expectNear(pixel_points[i].getX(), expected_x[i]);
      expectNear(pixel_points[i].getY(), expected_y[i]);
    }
  }

  TEST("updateX/YPixelPoints: update only some indices") {
    cmp::PlotLookAndFeel lnf;
    std::vector<float> x_data = {0.f, 2.5f, 5.f, 7.5f, 10.f};
    std::vector<float> y_data = {0.f, 2.5f, 5.f, 7.5f, 10.f};
    auto indices = makeIotaIndices(x_data.size());
    cmp::PixelPoints pixel_points(indices.size());

    lnf.updateXPixelPoints({}, cmp::Scaling::linear, {0.f, 10.f}, axes_bounds,
                           x_data, indices, pixel_points);
    lnf.updateYPixelPoints({}, cmp::Scaling::linear, {0.f, 10.f}, axes_bounds,
                           y_data, indices, pixel_points);

    x_data[2] = 2.5f;
    y_data[2] = 7.5f;
    lnf.updateXPixelPoints({2u}, cmp::Scaling::linear, {0.f, 10.f}, axes_bounds,
                           x_data, indices, pixel_points);
    lnf.updateYPixelPoints({2u}, cmp::Scaling::linear, {0.f, 10.f}, axes_bounds,
                           y_data, indices, pixel_points);

    // Index 2 is recalculated from the new data.
    expectNear(pixel_points[2].getX(), 125.f);
    expectNear(pixel_points[2].getY(), 100.f);
    // The other points are untouched.
    expectNear(pixel_points[1].getX(), 125.f);
    expectNear(pixel_points[1].getY(), 300.f);
    expectNear(pixel_points[3].getX(), 375.f);
    expectNear(pixel_points[3].getY(), 100.f);
  }

  TEST("Full pipeline: pixel points map back to the plotted data") {
    const std::vector<float> x_data = {0.f, 2.5f, 5.f, 7.5f, 10.f};
    const std::vector<float> y_data = {0.f, 5.f, 10.f, 5.f, 0.f};
    const auto x_lim = cmp::Lim_f(0.f, 10.f);
    const auto y_lim = cmp::Lim_f(0.f, 10.f);

    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);
    plot.plot({y_data}, {x_data});
    plot.xLim(x_lim.min, x_lim.max);
    plot.yLim(y_lim.min, y_lim.max);

    const auto series = getChildComponentHelper<cmp::Series>(plot);
    expectEquals(series.size(), 1ul);

    const auto& pixel_points = series[0]->getPixelPoints();
    const auto& pixel_point_indices = series[0]->getPixelPointIndices();
    expectEquals(pixel_points.size(), x_data.size());
    expectEquals(pixel_point_indices.size(), pixel_points.size());

    // Pixel points are local to the axes area, so the inverse conversion
    // uses the series's local bounds.
    const auto local_bounds = series[0]->getLocalBounds().toFloat();
    expect(!local_bounds.isEmpty());

    // Tolerance of one pixel expressed in data units.
    const auto x_tolerance = (x_lim.max - x_lim.min) / local_bounds.getWidth();
    const auto y_tolerance = (y_lim.max - y_lim.min) / local_bounds.getHeight();

    for (std::size_t i = 0; i < pixel_points.size(); ++i) {
      const auto data_point = cmp::getDataPointFromPixelCoordinate(
          pixel_points[i], local_bounds, x_lim, cmp::Scaling::linear, y_lim,
          cmp::Scaling::linear);

      expectWithinAbsoluteError(data_point.getX(),
                                x_data[pixel_point_indices[i]], x_tolerance);
      expectWithinAbsoluteError(data_point.getY(),
                                y_data[pixel_point_indices[i]], y_tolerance);
    }
  }
};
