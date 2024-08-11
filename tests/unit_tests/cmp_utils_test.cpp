#include <string>
#include <vector>

#include "cmp_test_helper.hpp"
#include "juce_core/system/juce_PlatformDefs.h"

#include "cmp_utils.h"

SECTION(UtilsTest, "Utils class"){TEST("Generate ticks: 0.0f -> 1000.0f"){
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
}
;