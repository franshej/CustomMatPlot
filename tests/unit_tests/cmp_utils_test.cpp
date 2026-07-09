#include <string>
#include <vector>

#include "cmp_test_helper.hpp"
#include "juce_core/system/juce_PlatformDefs.h"

#include "cmp_utils.h"

SECTION(UtilsTest, "Utils class"){

TEST("Generate ticks: 0.0f -> 1000.0f"){
    std::vector<float> result = cmp::TicksGenerator::generateTicks(
        0.0f, 1000.0f, 10, std::vector<float>());

std::vector<float> expected = {
    -500.0f, -400.0f, -300.0f, -200.0f, -100.0f, 0.0f,    100.0f,
    200.0f,  300.0f,  400.0f,  500.0f,  600.0f,  700.0f,  800.0f,
    900.0f,  1000.0f, 1100.0f, 1200.0f, 1300.0f, 1400.0f, 1500.0f};

expectEquals(result.size(), expected.size());
for (size_t i = 0; i < result.size(); ++i) {
  expectEquals(result[i], expected[i]);
}
}

TEST("Generate Ticks: -10.0f -> -1.0f") {
  std::vector<float> result = cmp::TicksGenerator::generateTicks(
      -10.0f, -1.0f, 10, std::vector<float>());
  std::vector<float> expected = {-15.0f, -14.0f, -13.0f, -12.0f, -11.0f,
                                 -10.0f, -9.0f,  -8.0f,  -7.0f,  -6.0f,
                                 -5.0f,  -4.0f,  -3.0f,  -2.0f,  -1.0f,
                                 0.0f,   1.0f,   2.0f,   3.0f,   4.0f};

  expectEquals(result.size(), expected.size());
  for (size_t i = 0; i < result.size(); ++i) {
    expectEquals(result[i], expected[i]);
  }
}

TEST("Generate Ticks: -10000.100f -> -10000.001f") {
  std::vector<float> result = cmp::TicksGenerator::generateTicks(
      -1000.100f, -1000.00f, 10, std::vector<float>());
  std::vector<float> expected = {-1000.150f, -1000.140f, -1000.130f, -1000.120f,
                                 -1000.110f, -1000.100f, -1000.090f, -1000.080f,
                                 -1000.070f, -1000.060f, -1000.050f, -1000.040f,
                                 -1000.030f, -1000.020f, -1000.010f, -1000.000f,
                                 -999.990f,  -999.980f,  -999.970f,  -999.960f};

  expectEquals(result.size(), expected.size());
  for (size_t i = 0; i < result.size(); ++i) {
    expectWithinAbsoluteError(result[i], expected[i], 0.009f);
  }
}

TEST("Previous Ticks") {
  std::vector<float>
      expected = {-1000.100f, -1000.090f, -1000.080f, -1000.070f,
                  9999,       -1000.050f, -1000.040f, -1000.030f,
                  -1000.020f, -1000.010f};  // Using 9999 as a test value to
                                            // make sure that the function
                                            // returns the previous vector

  std::vector<float> result =
      cmp::TicksGenerator::generateTicks(-1000.100f, -1000.00f, 10, expected);

  expectEquals(result.size(), expected.size());
  for (size_t i = 0; i < result.size(); ++i) {
    expectWithinAbsoluteError(result[i], expected[i], 0.009f);
  }
}

TEST("Float to string: 123.45000") {
  float num = 123.45000;
  std::string expected = "123.45";
  std::string result = cmp::valueToStringWithoutTrailingZeros(num);
  expectEquals(result, expected);
}

TEST("Float to string: 123.45") {
  float num = 123.45;
  std::string expected = "123.45";
  std::string result = cmp::valueToStringWithoutTrailingZeros(num);
  expectEquals(result, expected);
}

TEST("Float to string: 1000.0000") {
  float num = 1000.0000;
  std::string expected = "1000";
  std::string result = cmp::valueToStringWithoutTrailingZeros(num);
  expectEquals(result, expected);
}

TEST("Float to string: 0.00012345000") {
  float num = 0.00012345000f;
  std::string expected = "0.00012345";
  std::string result = cmp::valueToStringWithoutTrailingZeros(num);
  expectEquals(result, expected);
}

TEST("Float to string: 0.0") {
  float num = 0.0;
  std::string expected = "0";
  std::string result = cmp::valueToStringWithoutTrailingZeros(num);
  expectEquals(result, expected);
}
}
;

/* Characterization tests for the data <-> pixel coordinate conversions.
 *
 * These tests pin down the current behaviour of the conversion functions in
 * cmp_utils.h before they are refactored into a per-axis transform (see
 * docs/3d-plan.md). Two conventions worth noting:
 *
 * - The forward functions (getX/YPixelValue* together with
 *   getX/YScaleAndOffset) produce pixel values in axes-area-local
 *   coordinates, i.e. they only use the width/height of the axes bounds.
 * - The inverse functions (getX/YDataFromX/YPixelCoordinate) take absolute
 *   bounds and use the bounds origin.
 */
SECTION(CoordinateConversionTest, "Coordinate conversions") {
  auto expectNear = [&](const float result, const float expected) {
    expectWithinAbsoluteError(result, expected,
                              1e-3f * (1.0f + std::abs(expected)));
  };

  TEST("Pixel to x-data: linear") {
    const auto bounds = juce::Rectangle<float>(0.f, 0.f, 500.f, 400.f);
    const auto x_lim = cmp::Lim_f(0.f, 10.f);

    expectNear(cmp::getXDataFromXPixelCoordinate(0.f, bounds, x_lim,
                                                 cmp::Scaling::linear),
               0.f);
    expectNear(cmp::getXDataFromXPixelCoordinate(250.f, bounds, x_lim,
                                                 cmp::Scaling::linear),
               5.f);
    expectNear(cmp::getXDataFromXPixelCoordinate(500.f, bounds, x_lim,
                                                 cmp::Scaling::linear),
               10.f);
  }

  TEST("Pixel to x-data: linear with bounds offset") {
    const auto bounds = juce::Rectangle<float>(50.f, 30.f, 500.f, 400.f);
    const auto x_lim = cmp::Lim_f(0.f, 10.f);

    expectNear(cmp::getXDataFromXPixelCoordinate(50.f, bounds, x_lim,
                                                 cmp::Scaling::linear),
               0.f);
    expectNear(cmp::getXDataFromXPixelCoordinate(300.f, bounds, x_lim,
                                                 cmp::Scaling::linear),
               5.f);
    expectNear(cmp::getXDataFromXPixelCoordinate(550.f, bounds, x_lim,
                                                 cmp::Scaling::linear),
               10.f);
  }

  TEST("Pixel to x-data: logarithmic") {
    const auto bounds = juce::Rectangle<float>(0.f, 0.f, 300.f, 400.f);
    const auto x_lim = cmp::Lim_f(1.f, 1000.f);

    expectNear(cmp::getXDataFromXPixelCoordinate(0.f, bounds, x_lim,
                                                 cmp::Scaling::logarithmic),
               1.f);
    expectNear(cmp::getXDataFromXPixelCoordinate(100.f, bounds, x_lim,
                                                 cmp::Scaling::logarithmic),
               10.f);
    expectNear(cmp::getXDataFromXPixelCoordinate(200.f, bounds, x_lim,
                                                 cmp::Scaling::logarithmic),
               100.f);
    expectNear(cmp::getXDataFromXPixelCoordinate(300.f, bounds, x_lim,
                                                 cmp::Scaling::logarithmic),
               1000.f);
  }

  TEST("Pixel to y-data: linear (y-axis is inverted)") {
    const auto bounds = juce::Rectangle<float>(0.f, 0.f, 500.f, 400.f);
    const auto y_lim = cmp::Lim_f(0.f, 10.f);

    expectNear(cmp::getYDataFromYPixelCoordinate(0.f, bounds, y_lim,
                                                 cmp::Scaling::linear),
               10.f);
    expectNear(cmp::getYDataFromYPixelCoordinate(100.f, bounds, y_lim,
                                                 cmp::Scaling::linear),
               7.5f);
    expectNear(cmp::getYDataFromYPixelCoordinate(400.f, bounds, y_lim,
                                                 cmp::Scaling::linear),
               0.f);
  }

  TEST("Pixel to y-data: logarithmic") {
    const auto bounds = juce::Rectangle<float>(0.f, 0.f, 500.f, 300.f);
    const auto y_lim = cmp::Lim_f(1.f, 1000.f);

    expectNear(cmp::getYDataFromYPixelCoordinate(0.f, bounds, y_lim,
                                                 cmp::Scaling::logarithmic),
               1000.f);
    expectNear(cmp::getYDataFromYPixelCoordinate(100.f, bounds, y_lim,
                                                 cmp::Scaling::logarithmic),
               100.f);
    expectNear(cmp::getYDataFromYPixelCoordinate(200.f, bounds, y_lim,
                                                 cmp::Scaling::logarithmic),
               10.f);
    expectNear(cmp::getYDataFromYPixelCoordinate(300.f, bounds, y_lim,
                                                 cmp::Scaling::logarithmic),
               1.f);
  }

  TEST("X-data to pixel: linear") {
    const auto [x_scale, x_offset] =
        cmp::getXScaleAndOffset(500.f, {0.f, 10.f}, cmp::Scaling::linear);

    expectNear(cmp::getXPixelValueLinear(0.f, x_scale, x_offset), 0.f);
    expectNear(cmp::getXPixelValueLinear(5.f, x_scale, x_offset), 250.f);
    expectNear(cmp::getXPixelValueLinear(10.f, x_scale, x_offset), 500.f);
  }

  TEST("X-data to pixel: linear with negative limits") {
    const auto [x_scale, x_offset] =
        cmp::getXScaleAndOffset(500.f, {-5.f, 5.f}, cmp::Scaling::linear);

    expectNear(cmp::getXPixelValueLinear(-5.f, x_scale, x_offset), 0.f);
    expectNear(cmp::getXPixelValueLinear(0.f, x_scale, x_offset), 250.f);
    expectNear(cmp::getXPixelValueLinear(5.f, x_scale, x_offset), 500.f);
  }

  TEST("X-data to pixel: logarithmic") {
    const auto [x_scale, x_offset] =
        cmp::getXScaleAndOffset(300.f, {1.f, 1000.f}, cmp::Scaling::logarithmic);

    expectNear(cmp::getXPixelValueLogarithmic(1.f, x_scale, x_offset), 0.f);
    expectNear(cmp::getXPixelValueLogarithmic(10.f, x_scale, x_offset), 100.f);
    expectNear(cmp::getXPixelValueLogarithmic(1000.f, x_scale, x_offset),
               300.f);
  }

  TEST("Y-data to pixel: linear (y-axis is inverted)") {
    const auto [y_scale, y_offset] =
        cmp::getYScaleAndOffset(400.f, {0.f, 10.f}, cmp::Scaling::linear);

    expectNear(cmp::getYPixelValueLinear(0.f, y_scale, y_offset), 400.f);
    expectNear(cmp::getYPixelValueLinear(2.5f, y_scale, y_offset), 300.f);
    expectNear(cmp::getYPixelValueLinear(10.f, y_scale, y_offset), 0.f);
  }

  TEST("Y-data to pixel: logarithmic") {
    const auto [y_scale, y_offset] =
        cmp::getYScaleAndOffset(300.f, {1.f, 1000.f}, cmp::Scaling::logarithmic);

    expectNear(cmp::getYPixelValueLogarithmic(1.f, y_scale, y_offset), 300.f);
    expectNear(cmp::getYPixelValueLogarithmic(100.f, y_scale, y_offset), 100.f);
    expectNear(cmp::getYPixelValueLogarithmic(1000.f, y_scale, y_offset), 0.f);
  }

  TEST("Round trip: data -> pixel -> data") {
    const auto bounds = juce::Rectangle<float>(0.f, 0.f, 500.f, 400.f);
    const auto test_values = std::vector<float>{1.3f, 3.7f, 8.9f, 11.2f};

    const auto x_lim = cmp::Lim_f(1.f, 12.f);
    const auto y_lim = cmp::Lim_f(1.f, 12.f);

    for (const auto scaling :
         {cmp::Scaling::linear, cmp::Scaling::logarithmic}) {
      const auto [x_scale, x_offset] =
          cmp::getXScaleAndOffset(bounds.getWidth(), x_lim, scaling);
      const auto [y_scale, y_offset] =
          cmp::getYScaleAndOffset(bounds.getHeight(), y_lim, scaling);

      for (const auto value : test_values) {
        const auto x_pixel =
            scaling == cmp::Scaling::linear
                ? cmp::getXPixelValueLinear(value, x_scale, x_offset)
                : cmp::getXPixelValueLogarithmic(value, x_scale, x_offset);
        const auto y_pixel =
            scaling == cmp::Scaling::linear
                ? cmp::getYPixelValueLinear(value, y_scale, y_offset)
                : cmp::getYPixelValueLogarithmic(value, y_scale, y_offset);

        expectNear(
            cmp::getXDataFromXPixelCoordinate(x_pixel, bounds, x_lim, scaling),
            value);
        expectNear(
            cmp::getYDataFromYPixelCoordinate(y_pixel, bounds, y_lim, scaling),
            value);
      }
    }
  }

  TEST("Round trip: pixel -> data -> pixel") {
    const auto bounds = juce::Rectangle<float>(0.f, 0.f, 500.f, 400.f);
    const auto x_lim = cmp::Lim_f(0.5f, 200.f);
    const auto y_lim = cmp::Lim_f(0.5f, 200.f);

    for (const auto scaling :
         {cmp::Scaling::linear, cmp::Scaling::logarithmic}) {
      const auto [x_scale, x_offset] =
          cmp::getXScaleAndOffset(bounds.getWidth(), x_lim, scaling);
      const auto [y_scale, y_offset] =
          cmp::getYScaleAndOffset(bounds.getHeight(), y_lim, scaling);

      for (const auto pixel : {0.f, 123.f, 250.f, 399.f}) {
        const auto x_data =
            cmp::getXDataFromXPixelCoordinate(pixel, bounds, x_lim, scaling);
        const auto x_pixel =
            scaling == cmp::Scaling::linear
                ? cmp::getXPixelValueLinear(x_data, x_scale, x_offset)
                : cmp::getXPixelValueLogarithmic(x_data, x_scale, x_offset);
        expectWithinAbsoluteError(x_pixel, pixel, 0.1f);

        const auto y_data =
            cmp::getYDataFromYPixelCoordinate(pixel, bounds, y_lim, scaling);
        const auto y_pixel =
            scaling == cmp::Scaling::linear
                ? cmp::getYPixelValueLinear(y_data, y_scale, y_offset)
                : cmp::getYPixelValueLogarithmic(y_data, y_scale, y_offset);
        expectWithinAbsoluteError(y_pixel, pixel, 0.1f);
      }
    }
  }
}
;