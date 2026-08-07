#include <juce_core/juce_core.h>

#include <algorithm>
#include <numeric>
#include <span>
#include <vector>

#include "cmp_lookandfeel.h"
#include "cmp_plot.h"
#include "cmp_series.h"
#include "cmp_test_helper.hpp"

/* Tests for scoped write access to a series' y-values.
 *
 * writeY hands out a span over the series' own buffer, so writing through it
 * copies nothing. The span is as long as the series' x-data, which is what
 * makes the number of values impossible to get wrong.
 */
SECTION(WriteYTest, "Scoped y-write") {
  const auto seriesOf = [](cmp::Plot& plot) {
    return getChildComponentHelper<cmp::Series>(plot);
  };

  TEST("Writing through the handle updates the series") {
    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);
    plot.plot({.y = std::vector<float>{0.f, 0.f, 0.f, 0.f}});

    {
      auto y = plot.writeY();
      std::iota(y.values().begin(), y.values().end(), 1.f);
    }

    expectEqualVectors(seriesOf(plot).at(0)->getYData(),
                       std::vector<float>{1.f, 2.f, 3.f, 4.f},
                       [&](auto a, auto b) { expectEquals(a, b); });
  }

  TEST("The span is as long as the series, so the size cannot be wrong") {
    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);
    plot.plot({.y = std::vector<float>(320u, 1.f)});

    auto y = plot.writeY();

    expectEquals(y.values().size(), std::size_t(320u));
    expectEquals(y.values().size(), seriesOf(plot).at(0)->getXData().size());
  }

  TEST("It writes the series' own buffer, not a copy") {
    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);
    plot.plot({.y = std::vector<float>{5.f, 6.f, 7.f}});

    const auto* series_data = seriesOf(plot).at(0)->getYData().data();

    auto y = plot.writeY();

    expect(y.values().data() == series_data,
           "the span must alias the series' own storage");
  }

  TEST("Each series is addressed by its own index") {
    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);
    plot.plot({{.y = std::vector<float>{0.f, 0.f}},
               {.y = std::vector<float>{0.f, 0.f}}});

    {
      auto first = plot.writeY(0u);
      std::fill(first.values().begin(), first.values().end(), 11.f);
    }
    {
      auto second = plot.writeY(1u);
      std::fill(second.values().begin(), second.values().end(), 22.f);
    }

    const auto series = seriesOf(plot);
    expectEquals(series.size(), 2ul);
    expectEquals(series[0]->getYData().at(0), 11.f);
    expectEquals(series[1]->getYData().at(0), 22.f);
  }

  TEST("An out-of-range index yields an empty, invalid handle") {
    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);
    plot.plot({.y = std::vector<float>{1.f, 2.f}});

    auto y = plot.writeY(7u);

    expect(y.values().empty());
    expect(!y.isValid());
  }

  TEST("Replacing the series while a handle is alive does not dangle") {
    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);
    plot.plot({.y = std::vector<float>{1.f, 2.f, 3.f}});

    auto y = plot.writeY();
    expect(y.isValid());

    // The handle refers to its series by index and re-resolves it, so
    // clearing the plot leaves it empty rather than pointing at freed memory.
    plot.clear();

    expect(y.values().empty());
    expect(!y.isValid());
  }

  TEST("Repeated writes stay consistent") {
    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);
    plot.plot({.y = std::vector<float>(64u, 0.f)});

    for (auto pass = 0; pass < 5; ++pass) {
      auto y = plot.writeY();

      expectEquals(y.values().size(), std::size_t(64u));
      std::fill(y.values().begin(), y.values().end(), float(pass));
    }

    const auto& y_data = seriesOf(plot).at(0)->getYData();
    expectEquals(y_data.size(), std::size_t(64u));
    expectEquals(y_data.at(0), 4.f);
  }
}
