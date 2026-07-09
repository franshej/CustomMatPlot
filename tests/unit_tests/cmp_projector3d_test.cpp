#include <vector>

#include "cmp_projector3d.h"
#include "cmp_test_helper.hpp"

/* Tests for the 3D -> 2D projector.
 *
 * Pixel points are axes-area-local, like the 2D pixel-point pipeline: the
 * view-space bounding box of the data cube is stretched to fill the series
 * bounds, with the pixel y-axis pointing down.
 */
SECTION(Projector3DTest, "3D projector") {
  auto expectPointNear = [&](const juce::Point<float> result,
                             const juce::Point<float> expected) {
    expectWithinAbsoluteError(result.getX(), expected.getX(), 1e-2f);
    expectWithinAbsoluteError(result.getY(), expected.getY(), 1e-2f);
  };

  const auto axes_bounds = juce::Rectangle<int>(0, 0, 500, 400);

  const auto linear_axes =
      cmp::Axes3{{{0.f, 10.f}, cmp::Scaling::linear},
                 {{0.f, 10.f}, cmp::Scaling::linear},
                 {{0.f, 10.f}, cmp::Scaling::linear}};

  TEST("Top view: x maps across the width, y up the height") {
    const cmp::Camera3D top_view(0.f, 90.f);
    const cmp::Projector3D projector(linear_axes, top_view, axes_bounds);

    // (x_min, y_min) is the bottom-left corner of the axes area.
    expectPointNear(projector.toPixel({0.f, 0.f, 5.f}), {0.f, 400.f});
    expectPointNear(projector.toPixel({10.f, 10.f, 5.f}), {500.f, 0.f});
    expectPointNear(projector.toPixel({5.f, 5.f, 5.f}), {250.f, 200.f});
    // z does not affect the top view.
    expectPointNear(projector.toPixel({5.f, 5.f, 0.f}), {250.f, 200.f});
    expectPointNear(projector.toPixel({5.f, 5.f, 10.f}), {250.f, 200.f});
  }

  TEST("Front view: x maps across the width, z up the height") {
    const cmp::Camera3D front_view(0.f, 0.f);
    const cmp::Projector3D projector(linear_axes, front_view, axes_bounds);

    expectPointNear(projector.toPixel({0.f, 5.f, 0.f}), {0.f, 400.f});
    expectPointNear(projector.toPixel({10.f, 5.f, 10.f}), {500.f, 0.f});
    expectPointNear(projector.toPixel({2.5f, 5.f, 7.5f}), {125.f, 100.f});
    // y is the depth axis in the front view.
    expectPointNear(projector.toPixel({2.5f, 0.f, 7.5f}), {125.f, 100.f});
  }

  TEST("Logarithmic z-axis is normalized in log space") {
    const auto log_z_axes =
        cmp::Axes3{{{0.f, 10.f}, cmp::Scaling::linear},
                   {{0.f, 10.f}, cmp::Scaling::linear},
                   {{1.f, 1000.f}, cmp::Scaling::logarithmic}};
    const cmp::Camera3D front_view(0.f, 0.f);
    const cmp::Projector3D projector(log_z_axes, front_view, axes_bounds);

    // One decade per third of the series height.
    expectPointNear(projector.toPixel({5.f, 5.f, 1.f}), {250.f, 400.f});
    expectPointNear(projector.toPixel({5.f, 5.f, 10.f}), {250.f, 400.f * 2.f / 3.f});
    expectPointNear(projector.toPixel({5.f, 5.f, 100.f}), {250.f, 400.f / 3.f});
    expectPointNear(projector.toPixel({5.f, 5.f, 1000.f}), {250.f, 0.f});
  }

  TEST("Logarithmic x-axis is normalized in log space") {
    const auto log_x_axes =
        cmp::Axes3{{{1.f, 1000.f}, cmp::Scaling::logarithmic},
                   {{0.f, 10.f}, cmp::Scaling::linear},
                   {{0.f, 10.f}, cmp::Scaling::linear}};
    const cmp::Camera3D top_view(0.f, 90.f);
    const cmp::Projector3D projector(log_x_axes, top_view, axes_bounds);

    // x maps across the width, one decade per third.
    expectPointNear(projector.toPixel({1.f, 5.f, 5.f}), {0.f, 200.f});
    expectPointNear(projector.toPixel({10.f, 5.f, 5.f}), {500.f / 3.f, 200.f});
    expectPointNear(projector.toPixel({100.f, 5.f, 5.f}), {500.f * 2.f / 3.f, 200.f});
    expectPointNear(projector.toPixel({1000.f, 5.f, 5.f}), {500.f, 200.f});
  }

  TEST("Logarithmic y-axis is normalized in log space") {
    const auto log_y_axes =
        cmp::Axes3{{{0.f, 10.f}, cmp::Scaling::linear},
                   {{1.f, 1000.f}, cmp::Scaling::logarithmic},
                   {{0.f, 10.f}, cmp::Scaling::linear}};
    const cmp::Camera3D top_view(0.f, 90.f);
    const cmp::Projector3D projector(log_y_axes, top_view, axes_bounds);

    // y maps up the height (inverted), one decade per third.
    expectPointNear(projector.toPixel({5.f, 1.f, 5.f}), {250.f, 400.f});
    expectPointNear(projector.toPixel({5.f, 10.f, 5.f}), {250.f, 400.f * 2.f / 3.f});
    expectPointNear(projector.toPixel({5.f, 100.f, 5.f}), {250.f, 400.f / 3.f});
    expectPointNear(projector.toPixel({5.f, 1000.f, 5.f}), {250.f, 0.f});
  }

  TEST("Default view: the data cube fills the axes bounds") {
    const cmp::Projector3D projector(linear_axes, cmp::Camera3D(),
                                     axes_bounds);

    auto min_pixel = juce::Point<float>(1e9f, 1e9f);
    auto max_pixel = juce::Point<float>(-1e9f, -1e9f);

    for (const auto x : {0.f, 10.f}) {
      for (const auto y : {0.f, 10.f}) {
        for (const auto z : {0.f, 10.f}) {
          const auto pixel = projector.toPixel({x, y, z});

          min_pixel = {std::min(min_pixel.getX(), pixel.getX()),
                       std::min(min_pixel.getY(), pixel.getY())};
          max_pixel = {std::max(max_pixel.getX(), pixel.getX()),
                       std::max(max_pixel.getY(), pixel.getY())};
        }
      }
    }

    expectPointNear(min_pixel, {0.f, 0.f});
    expectPointNear(max_pixel, {500.f, 400.f});
  }

  TEST("updatePixelPoints projects all points") {
    const std::vector<float> x_data = {0.f, 5.f, 10.f};
    const std::vector<float> y_data = {0.f, 5.f, 10.f};
    const std::vector<float> z_data = {0.f, 5.f, 10.f};

    const cmp::Camera3D top_view(0.f, 90.f);
    const cmp::Projector3D projector(linear_axes, top_view, axes_bounds);

    cmp::PixelPoints pixel_points;
    projector.updatePixelPoints(x_data, y_data, z_data, pixel_points);

    expectEquals(pixel_points.size(), x_data.size());
    expectPointNear(pixel_points[0], {0.f, 400.f});
    expectPointNear(pixel_points[1], {250.f, 200.f});
    expectPointNear(pixel_points[2], {500.f, 0.f});
  }
}
;
