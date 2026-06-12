#include "cmp_axis_transform.h"
#include "cmp_test_helper.hpp"

/* Tests for the per-axis value <-> pixel transform.
 *
 * AxisTransform takes an explicit pixel range: pixel_min is the pixel of
 * axis.lim.min and pixel_max the pixel of axis.lim.max. An inverted axis
 * (like the y-axis) is expressed by passing pixel_min > pixel_max.
 *
 * The expected values mirror the characterization tests for the legacy x/y
 * conversion functions in cmp_utils_test.cpp, which are now wrappers around
 * this transform.
 */
SECTION(AxisTransformTest, "Axis transform") {
  auto expectNear = [&](const float result, const float expected) {
    expectWithinAbsoluteError(result, expected,
                              1e-3f * (1.0f + std::abs(expected)));
  };

  TEST("Linear: value to pixel") {
    const cmp::AxisTransform transform({{0.f, 10.f}, cmp::Scaling::linear},
                                       0.f, 500.f);

    expectNear(transform.toPixel(0.f), 0.f);
    expectNear(transform.toPixel(5.f), 250.f);
    expectNear(transform.toPixel(10.f), 500.f);
  }

  TEST("Linear: pixel to value") {
    const cmp::AxisTransform transform({{0.f, 10.f}, cmp::Scaling::linear},
                                       0.f, 500.f);

    expectNear(transform.fromPixel(0.f), 0.f);
    expectNear(transform.fromPixel(250.f), 5.f);
    expectNear(transform.fromPixel(500.f), 10.f);
  }

  TEST("Linear: negative limits") {
    const cmp::AxisTransform transform({{-5.f, 5.f}, cmp::Scaling::linear},
                                       0.f, 500.f);

    expectNear(transform.toPixel(-5.f), 0.f);
    expectNear(transform.toPixel(0.f), 250.f);
    expectNear(transform.fromPixel(250.f), 0.f);
    expectNear(transform.fromPixel(500.f), 5.f);
  }

  TEST("Logarithmic: value to pixel") {
    const cmp::AxisTransform transform(
        {{1.f, 1000.f}, cmp::Scaling::logarithmic}, 0.f, 300.f);

    expectNear(transform.toPixel(1.f), 0.f);
    expectNear(transform.toPixel(10.f), 100.f);
    expectNear(transform.toPixel(100.f), 200.f);
    expectNear(transform.toPixel(1000.f), 300.f);
  }

  TEST("Logarithmic: pixel to value") {
    const cmp::AxisTransform transform(
        {{1.f, 1000.f}, cmp::Scaling::logarithmic}, 0.f, 300.f);

    expectNear(transform.fromPixel(0.f), 1.f);
    expectNear(transform.fromPixel(100.f), 10.f);
    expectNear(transform.fromPixel(200.f), 100.f);
    expectNear(transform.fromPixel(300.f), 1000.f);
  }

  TEST("Inverted axis: pixel_min > pixel_max (y-axis convention)") {
    const cmp::AxisTransform linear({{0.f, 10.f}, cmp::Scaling::linear},
                                    400.f, 0.f);

    expectNear(linear.toPixel(0.f), 400.f);
    expectNear(linear.toPixel(2.5f), 300.f);
    expectNear(linear.toPixel(10.f), 0.f);
    expectNear(linear.fromPixel(0.f), 10.f);
    expectNear(linear.fromPixel(100.f), 7.5f);
    expectNear(linear.fromPixel(400.f), 0.f);

    const cmp::AxisTransform log({{1.f, 1000.f}, cmp::Scaling::logarithmic},
                                 300.f, 0.f);

    expectNear(log.toPixel(1.f), 300.f);
    expectNear(log.toPixel(100.f), 100.f);
    expectNear(log.fromPixel(0.f), 1000.f);
    expectNear(log.fromPixel(200.f), 10.f);
  }

  TEST("Pixel range with offset") {
    const cmp::AxisTransform transform({{0.f, 10.f}, cmp::Scaling::linear},
                                       50.f, 550.f);

    expectNear(transform.toPixel(0.f), 50.f);
    expectNear(transform.toPixel(5.f), 300.f);
    expectNear(transform.fromPixel(50.f), 0.f);
    expectNear(transform.fromPixel(550.f), 10.f);
  }

  TEST("Round trip: value -> pixel -> value") {
    for (const auto scaling :
         {cmp::Scaling::linear, cmp::Scaling::logarithmic}) {
      const cmp::AxisTransform transform({{1.f, 12.f}, scaling}, 0.f, 500.f);

      for (const auto value : {1.3f, 3.7f, 8.9f, 11.2f}) {
        expectNear(transform.fromPixel(transform.toPixel(value)), value);
      }
    }
  }
}
;
