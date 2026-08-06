#include <vector>

#include "cmp_camera3d.h"
#include "cmp_projector3d.h"
#include "cmp_test_helper.hpp"

/* Tests for the baseline a 3D gradient fill is closed along.
 *
 * The fill follows the floor of the data cube rather than the flat bottom of
 * the series bounds: the floor point of a sample sits where that sample's
 * (x, y) meets the xy-plane. Series3D derives that point from the already
 * projected one rather than projecting it again, so these tests pin down both
 * the geometry and the equivalence of the shortcut.
 */
SECTION(Series3DFillTest, "3D fill baseline") {
  const auto axes_bounds = juce::Rectangle<int>(0, 0, 500, 400);

  const auto axes = cmp::Axes3{{{0.f, 10.f}, cmp::Scaling::linear},
                               {{0.f, 10.f}, cmp::Scaling::linear},
                               {{0.f, 10.f}, cmp::Scaling::linear}};

  const auto expectPointNear = [&](const juce::Point<float> result,
                                   const juce::Point<float> expected) {
    expectWithinAbsoluteError(result.getX(), expected.getX(), 1e-2f);
    expectWithinAbsoluteError(result.getY(), expected.getY(), 1e-2f);
  };

  TEST("A point already on the floor is its own baseline") {
    const cmp::Projector3D projector(axes, cmp::Camera3D(), axes_bounds);

    // z is already at the minimum, so dropping it to the floor is a no-op.
    const auto point = juce::Point<float>(projector.toPixel({3.f, 7.f, 0.f}));
    const auto floor = juce::Point<float>(projector.toPixel({3.f, 7.f, 0.f}));

    expectPointNear(floor, point);
  }

  TEST("The floor point keeps the x/y of its sample") {
    const cmp::Projector3D projector(axes, cmp::Camera3D(), axes_bounds);

    // Two samples sharing (x, y) but at different heights must drop onto the
    // same point of the xy-plane.
    const auto floor_a = projector.toPixel({4.f, 6.f, 0.f});
    const auto floor_b = projector.toPixel({4.f, 6.f, 0.f});

    expectPointNear(floor_a, floor_b);

    // ...and that point differs from the sample drawn above it.
    const auto raised = projector.toPixel({4.f, 6.f, 10.f});
    expect(std::abs(raised.getY() - floor_a.getY()) > 1.0f,
           "a raised sample must not sit on its own floor point");
  }

  TEST("Height maps to screen height only, seen from the front") {
    // Front view: the screen y-axis is the z-axis, so the floor of every
    // sample lands at the bottom of the axes area.
    const cmp::Camera3D front_view(0.f, 0.f);
    const cmp::Projector3D projector(axes, front_view, axes_bounds);

    for (const auto x : {0.f, 5.f, 10.f}) {
      const auto floor = projector.toPixel({x, 5.f, 0.f});
      expectWithinAbsoluteError(floor.getY(), 400.f, 1e-2f);
    }
  }

  TEST("The derived floor matches projecting the floor point directly") {
    // Series3D derives the floor from the already-projected points instead of
    // projecting every point a second time. The shortcut must be exact, for
    // any camera and either axis scaling.
    const auto log_axes =
        cmp::Axes3{{{1.f, 1000.f}, cmp::Scaling::logarithmic},
                   {{0.f, 10.f}, cmp::Scaling::linear},
                   {{1.f, 1000.f}, cmp::Scaling::logarithmic}};

    for (const auto& current_axes : {axes, log_axes}) {
      const auto is_log = current_axes.z.scaling == cmp::Scaling::logarithmic;

      for (const auto azimuth : {-37.5f, 0.f, 45.f, 142.5f, -90.f}) {
        for (const auto elevation : {0.f, 30.f, 60.f, 90.f, -20.f}) {
          const cmp::Camera3D camera(azimuth, elevation);
          const cmp::Projector3D projector(current_axes, camera, axes_bounds);

          const auto pixels_per_unit_height = projector.pixelsPerUnitHeight();
          const auto z_floor = current_axes.z.lim.min;

          for (const auto t : {0.f, 0.25f, 0.5f, 0.75f, 1.f}) {
            const auto x = is_log ? std::pow(10.f, 3.f * t) : t * 10.f;
            const auto y = t * 10.f;
            const auto z = is_log ? std::pow(10.f, 3.f * t) : t * 10.f;

            const auto point = projector.toPixel({x, y, z});

            const auto derived = juce::Point<float>(
                point.getX(), point.getY() + projector.toUnitHeight(z) *
                                                 pixels_per_unit_height);

            const auto projected = projector.toPixel({x, y, z_floor});

            expectPointNear(derived, projected);
          }
        }
      }
    }
  }

  TEST("Seen from the top, a sample and its floor point coincide") {
    // Top view looks straight down the z-axis, so height is not visible and
    // the fill collapses onto the line.
    const cmp::Camera3D top_view(0.f, 90.f);
    const cmp::Projector3D projector(axes, top_view, axes_bounds);

    const auto point = projector.toPixel({2.f, 8.f, 10.f});
    const auto floor = projector.toPixel({2.f, 8.f, 0.f});

    expectPointNear(floor, point);
  }
}
