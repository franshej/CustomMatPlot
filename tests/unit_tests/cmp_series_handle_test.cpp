#include <juce_core/juce_core.h>

#include <algorithm>
#include <array>
#include <numeric>
#include <span>
#include <vector>

#include "cmp_lookandfeel.h"
#include "cmp_plot.h"
#include "cmp_series.h"
#include "cmp_test_helper.hpp"

/* Tests for the handles returned by plot.
 *
 * A handle is what makes the ordering safe: there is no way to update a
 * series without first having plotted one, because the update lives on what
 * plot returns.
 */
SECTION(SeriesHandleTest, "Series handles") {
  const auto seriesOf = [](cmp::Plot& plot) {
    return getChildComponentHelper<cmp::Series>(plot);
  };

  const auto make = [](const std::size_t n, const float value) {
    return std::vector<float>(n, value);
  };

  TEST("Plotting one series returns a handle to it") {
    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);

    const auto series = plot.plot({.y = make(8u, 1.f)});

    expect(series.isValid());
    expectEquals(series.size(), std::size_t(8u));
  }

  TEST("The handle sets the y-values") {
    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);

    const auto series = plot.plot({.y = make(4u, 1.f)});
    series.setY(std::vector<float>{5.f, 6.f, 7.f, 8.f});

    expectEqualVectors(seriesOf(plot).at(0)->getYData(),
                       std::vector<float>{5.f, 6.f, 7.f, 8.f},
                       [&](auto a, auto b) { expectEquals(a, b); });
  }

  TEST("The handle writes the series' own buffer, without copying") {
    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);

    const auto series = plot.plot({.y = make(16u, 0.f)});
    const auto* buffer = seriesOf(plot).at(0)->getYData().data();

    auto y = series.write();

    expect(y.values().data() == buffer,
           "the span must alias the series' own storage");
    expectEquals(y.values().size(), std::size_t(16u));
  }

  TEST("Plotting a list returns a handle per series") {
    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);

    const auto series = plot.plot({{.y = make(4u, 1.f)}, {.y = make(4u, 2.f)}});

    expectEquals(series.size(), std::size_t(2u));

    series[0].setY(make(4u, 11.f));
    series[1].setY(make(4u, 22.f));

    expectEquals(seriesOf(plot).at(0)->getYData().at(0), 11.f);
    expectEquals(seriesOf(plot).at(1)->getYData().at(0), 22.f);
  }

  TEST("The handles can be iterated") {
    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);

    const auto series = plot.plot({{.y = make(4u, 1.f)}, {.y = make(4u, 2.f)}});

    auto count = std::size_t{0};
    for (const auto one : series) {
      expectEquals(one.size(), std::size_t(4u));
      ++count;
    }

    expectEquals(count, std::size_t(2u));
  }

  TEST("A handle can be re-acquired after being discarded") {
    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);

    plot.plot({.y = make(4u, 1.f)});  // handle deliberately dropped

    const auto series = plot.series(0u);

    expect(series.isValid());
    series.setY(make(4u, 9.f));

    expectEquals(seriesOf(plot).at(0)->getYData().at(0), 9.f);
  }

  TEST("An index with no series gives an invalid handle") {
    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);

    plot.plot({.y = make(4u, 1.f)});

    const auto missing = plot.series(7u);

    expect(!missing.isValid());
    expectEquals(missing.size(), std::size_t(0u));

    // Updating through it must be a no-op rather than a crash.
    missing.setY(make(4u, 1.f));
  }

  TEST("Plotting again while a handle is held does not dangle") {
    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);

    const auto series = plot.plot({.y = make(4u, 1.f)});
    expect(series.isValid());

    plot.clear();

    expect(!series.isValid());
    expectEquals(series.size(), std::size_t(0u));
  }

  TEST("An update batches the series refresh until it closes") {
    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);

    const auto series =
        plot.plot({{.y = make(64u, 0.f)}, {.y = make(64u, 0.f)}});

    // Fixed limits, so a change in the values actually moves the pixel points
    // instead of being normalised away by autoscaling.
    plot.setYLim(0.f, 1000.f);

    // The pixel points are recomputed when a series update is committed, so
    // they show whether the work was deferred.
    const auto first_pixel_y = [&] {
      return seriesOf(plot).at(0)->getPixelPoints().at(0).getY();
    };

    const auto before = first_pixel_y();

    {
      auto update = plot.beginUpdate();

      series[0].setY(make(64u, 500.f));
      series[1].setY(make(64u, 500.f));

      // Still the old projection: the refresh is waiting for the update to
      // close.
      expectEquals(first_pixel_y(), before);
    }

    // Closing the update refreshed it once, for both series.
    expect(first_pixel_y() != before,
           "closing the update must refresh the series");
  }

  TEST("Nested updates commit once, at the outermost") {
    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);

    const auto series = plot.plot({.y = make(32u, 0.f)});

    plot.setYLim(0.f, 1000.f);

    const auto first_pixel_y = [&] {
      return seriesOf(plot).at(0)->getPixelPoints().at(0).getY();
    };

    const auto before = first_pixel_y();

    {
      auto outer = plot.beginUpdate();
      {
        auto inner = plot.beginUpdate();
        series.setY(make(32u, 250.f));
      }

      // The inner one closing must not have committed.
      expectEquals(first_pixel_y(), before);
    }

    expect(first_pixel_y() != before, "the outermost update must commit");
  }

  TEST("Setting every series at once batches without being asked to") {
    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);

    const auto series = plot.plot({{.y = make(8u, 0.f)}, {.y = make(8u, 0.f)}});

    plot.setYLim(0.f, 1000.f);

    const auto first_pixel_y = [&] {
      return seriesOf(plot).at(0)->getPixelPoints().at(0).getY();
    };

    const auto before = first_pixel_y();

    const auto left = make(8u, 400.f);
    const auto right = make(8u, 600.f);
    const std::array<std::span<const float>, 2> channels{
        std::span<const float>(left), std::span<const float>(right)};

    series.setY(channels);

    expectEquals(seriesOf(plot).at(0)->getYData().at(0), 400.f);
    expectEquals(seriesOf(plot).at(1)->getYData().at(0), 600.f);
    expect(first_pixel_y() != before, "the series must have been refreshed");
  }

  TEST("An update that changes nothing does not force a refresh") {
    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);

    plot.plot({.y = make(8u, 1.f)});

    // Opening and closing without touching a series is harmless.
    { auto update = plot.beginUpdate(); }

    expectEquals(seriesOf(plot).at(0)->getYData().size(), std::size_t(8u));
  }
}
