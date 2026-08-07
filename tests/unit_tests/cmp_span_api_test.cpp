#include <juce_core/juce_core.h>

#include <array>
#include <span>
#include <vector>

#include "cmp_lookandfeel.h"
#include "cmp_plot.h"
#include "cmp_series.h"
#include "cmp_test_helper.hpp"

/* Tests for taking the y-values as a span.
 *
 * The values are copied straight into the series, so they can come from any
 * contiguous range without first being packed into a vector.
 */
SECTION(SpanApiTest, "Span y-data") {
  const auto firstSeries = [](cmp::Plot& plot) {
    return getChildComponentHelper<cmp::Series>(plot).at(0);
  };

  TEST("A vector still works unchanged") {
    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);

    plot.plot({.y = std::vector<float>{1.f, 2.f, 3.f, 4.f}});
    plot.plotUpdateYOnly(std::vector<float>{5.f, 6.f, 7.f, 8.f});

    expectEqualVectors(firstSeries(plot)->getYData(),
                       std::vector<float>{5.f, 6.f, 7.f, 8.f},
                       [&](auto a, auto b) { expectEquals(a, b); });
  }

  TEST("A braced list still works unchanged") {
    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);

    plot.plot({.y = std::vector<float>{1.f, 2.f, 3.f, 4.f}});
    plot.plotUpdateYOnly({5.f, 6.f, 7.f, 8.f});

    expectEqualVectors(firstSeries(plot)->getYData(),
                       std::vector<float>{5.f, 6.f, 7.f, 8.f},
                       [&](auto a, auto b) { expectEquals(a, b); });
  }

  TEST("A std::array needs no vector") {
    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);

    plot.plot({.y = std::vector<float>{1.f, 2.f, 3.f, 4.f}});

    const std::array<float, 4> values{9.f, 8.f, 7.f, 6.f};
    plot.plotUpdateYOnly(values);

    expectEqualVectors(firstSeries(plot)->getYData(),
                       std::vector<float>{9.f, 8.f, 7.f, 6.f},
                       [&](auto a, auto b) { expectEquals(a, b); });
  }

  TEST("A raw buffer needs no vector") {
    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);

    plot.plot({.y = std::vector<float>{1.f, 2.f, 3.f, 4.f}});

    float buffer[4] = {2.f, 4.f, 6.f, 8.f};
    plot.plotUpdateYOnly(std::span<const float>(buffer, 4));

    expectEqualVectors(firstSeries(plot)->getYData(),
                       std::vector<float>{2.f, 4.f, 6.f, 8.f},
                       [&](auto a, auto b) { expectEquals(a, b); });
  }

  TEST("A sub-range of a larger buffer can be plotted directly") {
    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);

    plot.plot({.y = std::vector<float>{0.f, 0.f, 0.f}});

    // The middle three of a longer buffer, with no intermediate copy.
    const std::vector<float> big{1.f, 2.f, 3.f, 4.f, 5.f, 6.f};
    plot.plotUpdateYOnly(std::span<const float>(big).subspan(2u, 3u));

    expectEqualVectors(firstSeries(plot)->getYData(),
                       std::vector<float>{3.f, 4.f, 5.f},
                       [&](auto a, auto b) { expectEquals(a, b); });
  }

  TEST("A changed number of values keeps x and y the same length") {
    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);

    plot.plot({.y = std::vector<float>(200u, 1.f)});
    plot.plotUpdateYOnly(std::vector<float>(20u, 2.f));

    const auto* series = firstSeries(plot);

    expectEquals(series->getYData().size(), std::size_t(20u));
    expectEquals(series->getXData().size(), std::size_t(20u));
  }
}
