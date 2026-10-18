#include <juce_core/juce_core.h>

#include <utility>
#include <vector>

#include "cmp_lookandfeel.h"
#include "cmp_plot.h"
#include "cmp_series.h"
#include "cmp_test_helper.hpp"

/* Tests for the plot overloads that take ownership.
 *
 * A caller handing over series it no longer needs has its x- and y-values
 * moved into the plot rather than copied.
 */
SECTION(PlotSinkTest, "Plot sink overloads") {
  const auto seriesOf = [](cmp::Plot& plot) {
    return getChildComponentHelper<cmp::Series>(plot);
  };

  const auto expectEqualsLambda = [&](auto a, auto b) { expectEquals(a, b); };

  TEST("Moving a single series plots the same values") {
    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);

    cmp::SeriesData series{.x = {1.f, 2.f, 3.f}, .y = {4.f, 5.f, 6.f}};
    plot.plot(std::move(series));

    const auto plotted = seriesOf(plot);
    expectEquals(plotted.size(), 1ul);
    expectEqualVectors(plotted[0]->getXData(), {1.f, 2.f, 3.f},
                       expectEqualsLambda);
    expectEqualVectors(plotted[0]->getYData(), {4.f, 5.f, 6.f},
                       expectEqualsLambda);
  }

  TEST("Moving takes the caller's storage rather than copying it") {
    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);

    cmp::SeriesData series{.y = std::vector<float>(256u, 7.f)};
    const auto* source = series.y.data();

    plot.plot(std::move(series));

    // The series now holds the very buffer the caller allocated.
    expect(seriesOf(plot).at(0)->getYData().data() == source,
           "the plot must take the caller's buffer, not copy it");
  }

  TEST("Moving a list plots every series") {
    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);

    cmp::SeriesDataList list{{.y = {1.f, 2.f}}, {.y = {3.f, 4.f}}};
    plot.plot(std::move(list));

    const auto plotted = seriesOf(plot);
    expectEquals(plotted.size(), 2ul);
    expectEqualVectors(plotted[0]->getYData(), {1.f, 2.f}, expectEqualsLambda);
    expectEqualVectors(plotted[1]->getYData(), {3.f, 4.f}, expectEqualsLambda);
  }

  TEST("A moved series still gets its x-ramp generated") {
    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);

    plot.plot(cmp::SeriesData{.y = {9.f, 9.f, 9.f, 9.f}});

    expectEqualVectors(seriesOf(plot).at(0)->getXData(), {1.f, 2.f, 3.f, 4.f},
                       expectEqualsLambda);
  }

  TEST("Copying still works and leaves the caller's series intact") {
    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);

    const cmp::SeriesData series{.x = {1.f, 2.f}, .y = {8.f, 9.f}};
    plot.plot(series);

    // The const overload must not have disturbed the caller's data.
    expectEquals(series.x.size(), std::size_t(2u));
    expectEquals(series.y.size(), std::size_t(2u));

    expectEqualVectors(seriesOf(plot).at(0)->getYData(), {8.f, 9.f},
                       expectEqualsLambda);
  }

  TEST("A mismatched x and y still throws when moved") {
    cmp::Plot plot;
    plot.setBounds(0, 0, 500, 400);

    auto plotMismatched = [&] {
      plot.plot(cmp::SeriesData{.x = {1.f, 2.f, 3.f}, .y = {1.f, 2.f}});
    };

    try {
      plotMismatched();
      expect(false, "a mismatched x and y must throw");
    } catch (const std::invalid_argument&) {
      expect(true);
    }
  }
}
