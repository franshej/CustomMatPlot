#include "cmp_plot.h"

#include <juce_core/juce_core.h>
#include <memory>

#include "cmp_datamodels.h"
#include "cmp_trace.h"
#include "cmp_test_helper.hpp"

using namespace cmp;

SECTION(TraceClass, "Trace class") {
  const std::vector<float> y_data = {1.f, 2.f, 3.f, 4.f};
  cmp::Plot plot;
  plot.setBounds(0, 0, 100, 100);
  plot.xLim(1.f, 4.f);
  plot.yLim(1.f, 4.f);

  TEST("Empty trace points") {
    auto trace_points = getChildComponentHelper<TracePoint_f>(plot);
    expect(trace_points.empty());
  }

  TEST("Add single trace point") {
    plot.plot({y_data});
    plot.setTracePoint({1.f, 1.f});
    auto trace_points = getChildComponentHelper<TracePoint_f>(plot);
    expect(!trace_points.empty());
    expectEquals(trace_points.size(), 1ul);
    expect(trace_points[0]->getDataPoint() == juce::Point<float>(1.f, 1.f));
  }

  TEST("Add multiple trace points") {
    plot.setTracePoint({3.f, 3.f});
    auto trace_points = getChildComponentHelper<TracePoint_f>(plot);
    expect(!trace_points.empty());
    expectEquals(trace_points.size(), 2ul);
    expect(trace_points[0]->getDataPoint() == juce::Point<float>(1.f, 1.f));
    expect(trace_points[1]->getDataPoint() == juce::Point<float>(3.f, 3.f));
  }

  TEST("Update existing plot with same plot data") {
    plot.plot({{1.f, 2.f, 3.f, 4.f}});
    auto trace_points = getChildComponentHelper<TracePoint_f>(plot);
    expect(!trace_points.empty());
    expectEquals(trace_points.size(), 2ul);
    expect(trace_points[0]->getDataPoint() == juce::Point<float>(1.f, 1.f));
    expect(trace_points[1]->getDataPoint() == juce::Point<float>(3.f, 3.f));
  }

  TEST("Update existing plot points") {
    plot.plot({{0.f, 0.f, 0.f, 0.f}});
    auto trace_points = getChildComponentHelper<TracePoint_f>(plot);
    expect(!trace_points.empty());
    expectEquals(trace_points.size(), 2ul);
    expect(trace_points[0]->getDataPoint() == juce::Point<float>(1.f, 0.f));
    expect(trace_points[1]->getDataPoint() == juce::Point<float>(3.f, 0.f));
  }

  TEST("Update existing plot Y only") {
    plot.plotUpdateYOnly({{4.f, 3.f, 2.f, 1.f}});
    auto trace_points = getChildComponentHelper<TracePoint_f>(plot);
    expect(!trace_points.empty());
    expectEquals(trace_points.size(), 2ul);
    expect(trace_points[0]->getDataPoint() == juce::Point<float>(1.f, 4.f));
    expect(trace_points[1]->getDataPoint() == juce::Point<float>(3.f, 2.f));
  }

  TEST("Remove all trace points") {
    plot.clearTracePoints();
    auto trace_points = getChildComponentHelper<TracePoint_f>(plot);
    expect(trace_points.empty());
  }
}
