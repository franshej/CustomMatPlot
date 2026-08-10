#include <juce_core/juce_core.h>

#include <array>
#include <span>
#include <vector>

#include "cmp_lookandfeel.h"
#include "cmp_plot.h"
#include "cmp_series.h"
#include "cmp_test_helper.hpp"

/* Tests for updating a series' y-values when the number of them changes.
 *
 * Updating only the y-values skips the x-data update, which is where its
 * speed comes from. If the caller passes a different number of values,
 * keeping the old x-data would leave it - and the pixel-point indices derived
 * from it - describing a different number of points than the series holds, so
 * indices would be used to read past the end of the y-data. The x-data is
 * regenerated in that case instead.
 */
SECTION(UpdateYOnlySizeTest, "Handle setY size change") {
  const auto make = [](const std::size_t n, const float value) {
    return std::vector<float>(n, value);
  };

  /** Every index used to read the y-data must be inside it. */
  const auto countOutOfRangeIndices = [](const cmp::Series& series) {
    const auto& y_data = series.getYData();

    std::size_t out_of_range = 0u;
    for (const auto i : series.getPixelPointIndices())
      if (i >= y_data.size()) ++out_of_range;

    return out_of_range;
  };

  TEST("The same number of values keeps the existing x-data") {
    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);

    const std::vector<float> x_data = make(500u, 3.f);
    plot.plot({.x = x_data, .y = make(500u, 1.f)});
    plot.series(0u).setY(make(500u, 2.f));

    const auto series = getChildComponentHelper<cmp::Series>(plot);
    expectEquals(series.size(), 1ul);

    // The x-data the caller set is untouched, which is the whole point of
    // updating only the y-data.
    expectEqualVectors(series[0]->getXData(), x_data,
                       [&](auto a, auto b) { expectEquals(a, b); });
    expectEquals(series[0]->getYData().size(), std::size_t(500u));
    expectEquals(countOutOfRangeIndices(*series[0]), std::size_t(0u));
  }

  TEST("Fewer values leaves no index past the end of the y-data") {
    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);

    plot.plot({.y = make(500u, 1.f)});
    plot.series(0u).setY(make(10u, 2.f));

    const auto series = getChildComponentHelper<cmp::Series>(plot);
    expectEquals(series.size(), 1ul);

    expectEquals(series[0]->getYData().size(), std::size_t(10u));
    expectEquals(series[0]->getXData().size(), std::size_t(10u));
    expectEquals(countOutOfRangeIndices(*series[0]), std::size_t(0u));
  }

  TEST("More values keeps the x- and y-data the same length") {
    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);

    plot.plot({.y = make(10u, 1.f)});
    plot.series(0u).setY(make(500u, 2.f));

    const auto series = getChildComponentHelper<cmp::Series>(plot);

    expectEquals(series[0]->getYData().size(), std::size_t(500u));
    expectEquals(series[0]->getXData().size(), std::size_t(500u));
    expectEquals(countOutOfRangeIndices(*series[0]), std::size_t(0u));
  }

  TEST("Shrinking then growing again stays consistent") {
    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);

    plot.plot({.y = make(300u, 1.f)});

    for (const std::size_t n : {50u, 700u, 20u, 300u, 1u}) {
      plot.series(0u).setY(make(n, 2.f));

      const auto series = getChildComponentHelper<cmp::Series>(plot);

      expectEquals(series[0]->getYData().size(), n);
      expectEquals(series[0]->getXData().size(), n);
      expectEquals(countOutOfRangeIndices(*series[0]), std::size_t(0u));
    }
  }

  TEST("Multiple series each track their own size") {
    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);

    plot.plot({{.y = make(200u, 1.f)}, {.y = make(200u, 2.f)}});
    const auto first = make(30u, 3.f);
    const auto second = make(30u, 4.f);
    const std::array<std::span<const float>, 2> channels{
        std::span<const float>(first), std::span<const float>(second)};

    plot.series().setY(channels);

    const auto series = getChildComponentHelper<cmp::Series>(plot);
    expectEquals(series.size(), 2ul);

    for (const auto* one : series) {
      expectEquals(one->getYData().size(), std::size_t(30u));
      expectEquals(one->getXData().size(), std::size_t(30u));
      expectEquals(countOutOfRangeIndices(*one), std::size_t(0u));
    }
  }
}
