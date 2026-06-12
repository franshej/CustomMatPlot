#include "cmp_plot.h"

#include <juce_core/juce_core.h>
#include <memory>

#include "cmp_datamodels.h"
#include "cmp_graph_line.h"
#include "cmp_lookandfeel.h"
#include "cmp_trace.h"
#include "cmp_test_helper.hpp"
#include "cmp_utils.h"

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

/* Lookandfeel that maps dragging a tracepoint to moving the selected trace
 * points, like the 'move_pixel_points' example does.
 *
 * Note: PlotLookAndFeel::getUserInputAction caches the action map in a
 * function-local static, so the first lookandfeel that dispatches a mouse
 * action in this test binary decides the map. This is currently the only
 * test that dispatches mouse events. */
class MoveTracePointsLookAndFeel : public cmp::PlotLookAndFeel {
  std::map<cmp::UserInput, cmp::UserInputAction> overrideUserInputMapAction(
      std::map<cmp::UserInput, cmp::UserInputAction>
          default_user_input_map_action) const noexcept override {
    auto new_map = default_user_input_map_action;

    new_map[cmp::UserInput::left | cmp::UserInput::drag |
            cmp::UserInput::tracepoint] =
        cmp::UserInputAction::move_selected_trace_points;

    return new_map;
  }
};

SECTION(MoveSelectedTracePointsTest, "Move selected trace points") {
  auto makeMouseEvent = [](juce::Component* component,
                           const juce::Point<float> position,
                           const bool was_dragged) {
    return juce::MouseEvent(
        juce::Desktop::getInstance().getMainMouseSource(), position,
        juce::ModifierKeys::leftButtonModifier, 0.f, 0.f, 0.f, 0.f, 0.f,
        component, component, juce::Time::getCurrentTime(), position,
        juce::Time::getCurrentTime(), 1, was_dragged);
  };

  TEST("Drag a selected trace point: logarithmic x, linear y") {
    const std::vector<float> x_data = {1.f, 10.f, 100.f, 1000.f};
    const std::vector<float> y_data = {1.f, 2.f, 3.f, 4.f};
    const auto x_lim = cmp::Lim_f(1.f, 1000.f);
    const auto y_lim = cmp::Lim_f(0.f, 5.f);

    MoveTracePointsLookAndFeel lnf;
    cmp::Plot plot{cmp::Scaling::logarithmic, cmp::Scaling::linear};
    plot.setLookAndFeel(&lnf);
    plot.setBounds(0, 0, 500, 400);
    plot.setVisible(true);
    plot.setMovePointsType(cmp::PixelPointMoveType::horizontal_vertical);
    plot.plot({y_data}, {x_data});
    plot.xLim(x_lim.min, x_lim.max);
    plot.yLim(y_lim.min, y_lim.max);

    // Create a tracepoint on the data point (10, 2) (data index 1).
    plot.setTracePoint({10.f, 2.f});
    const auto trace_points = getChildComponentHelper<TracePoint_f>(plot);
    expectEquals(trace_points.size(), 1ul);
    auto* trace_point = trace_points[0];

    const auto graph_lines = getChildComponentHelper<cmp::GraphLine>(plot);
    expectEquals(graph_lines.size(), 1ul);

    // The graph line component covers the graph area.
    const auto graph_bounds = graph_lines[0]->getBounds();
    expect(!graph_bounds.isEmpty());
    // The bug only shows when the graph area does not start at the origin.
    expect(graph_bounds.getPosition() != juce::Point<int>(0, 0));

    // Mouse positions are relative to the tracepoint component.
    const auto mouse_down_pos = juce::Point<float>(3.f, 3.f);
    const auto mouse_drag_pos = mouse_down_pos + juce::Point<float>(50.f, 20.f);

    // Graph-area-local positions as Plot::getMousePositionRelativeToGraphArea
    // computes them (positions are rounded to int pixels).
    const auto trace_point_pos = trace_point->getBounds().getPosition();
    const auto prev_pos_local =
        (mouse_down_pos.toInt() + trace_point_pos - graph_bounds.getPosition())
            .toFloat();
    const auto new_pos_local =
        (mouse_drag_pos.toInt() + trace_point_pos - graph_bounds.getPosition())
            .toFloat();

    // The expected data movement converts the graph-area-local pixel
    // positions with graph-area-local (zero-origin) bounds.
    const auto local_bounds = graph_bounds.withZeroOrigin().toFloat();
    const auto d_x = cmp::getXDataFromXPixelCoordinate(
                         new_pos_local.getX(), local_bounds, x_lim,
                         cmp::Scaling::logarithmic) -
                     cmp::getXDataFromXPixelCoordinate(
                         prev_pos_local.getX(), local_bounds, x_lim,
                         cmp::Scaling::logarithmic);
    const auto d_y = cmp::getYDataFromYPixelCoordinate(
                         new_pos_local.getY(), local_bounds, y_lim,
                         cmp::Scaling::linear) -
                     cmp::getYDataFromYPixelCoordinate(
                         prev_pos_local.getY(), local_bounds, y_lim,
                         cmp::Scaling::linear);
    const auto expected_x = x_data[1] + d_x;
    const auto expected_y = y_data[1] + d_y;

    // Select the tracepoint and drag it.
    plot.mouseDown(makeMouseEvent(trace_point, mouse_down_pos, false));
    plot.mouseDrag(makeMouseEvent(trace_point, mouse_drag_pos, true));

    expectWithinAbsoluteError(graph_lines[0]->getXData()[1], expected_x,
                              1e-3f * expected_x);
    expectWithinAbsoluteError(graph_lines[0]->getYData()[1], expected_y,
                              1e-3f);

    // The other data points are untouched.
    expectEquals(graph_lines[0]->getXData()[0], x_data[0]);
    expectEquals(graph_lines[0]->getYData()[2], y_data[2]);

    plot.setLookAndFeel(nullptr);
  }
}
