#include "cmp_plot3d.h"

#include <vector>

#include "cmp_axes3dbox.h"
#include "cmp_graph_line3d.h"
#include "cmp_lookandfeel.h"
#include "cmp_test_helper.hpp"

SECTION(Plot3DClass, "Plot3D class") {
  auto expectEqualsLambda = [&](auto a, auto b) { expectEquals(a, b); };
  auto expectPointNear = [&](const juce::Point<float> result,
                             const juce::Point<float> expected) {
    expectWithinAbsoluteError(result.getX(), expected.getX(), 1e-2f);
    expectWithinAbsoluteError(result.getY(), expected.getY(), 1e-2f);
  };

  const std::vector<float> x_data1 = {0.f, 5.f, 10.f};
  const std::vector<float> y_data1 = {0.f, 5.f, 10.f};
  const std::vector<float> z_data1 = {0.f, 5.f, 10.f};
  const std::vector<float> x_data2 = {1.f, 2.f};
  const std::vector<float> y_data2 = {3.f, 4.f};
  const std::vector<float> z_data2 = {5.f, 6.f};

  cmp::Plot3D plot;

  TEST("Empty plot") {
    auto graph_lines = getChildComponentHelper<cmp::GraphLine3D>(plot);
    expect(graph_lines.empty());
  }

  TEST("Single graph line") {
    plot.plot3({x_data1}, {y_data1}, {z_data1});

    const auto graph_lines = getChildComponentHelper<cmp::GraphLine3D>(plot);
    expectEquals(graph_lines.size(), 1ul);
    expectEqualVectors(graph_lines[0]->getXData(), x_data1,
                       expectEqualsLambda);
    expectEqualVectors(graph_lines[0]->getYData(), y_data1,
                       expectEqualsLambda);
    expectEqualVectors(graph_lines[0]->getZData(), z_data1,
                       expectEqualsLambda);
    // The colour is assigned from the lookandfeel graph colours.
    expect(graph_lines[0]->getColour() != juce::Colour());
  }

  TEST("Several graph lines") {
    plot.plot3({x_data1, x_data2}, {y_data1, y_data2}, {z_data1, z_data2});

    const auto graph_lines = getChildComponentHelper<cmp::GraphLine3D>(plot);
    expectEquals(graph_lines.size(), 2ul);
    expectEqualVectors(graph_lines[1]->getXData(), x_data2,
                       expectEqualsLambda);
    expectEqualVectors(graph_lines[1]->getZData(), z_data2,
                       expectEqualsLambda);
    expect(graph_lines[0]->getColour() != graph_lines[1]->getColour());
  }

  TEST("Mismatched number of lines throws") {
    auto exception_thrown = false;

    try {
      plot.plot3({x_data1}, {y_data1, y_data2}, {z_data1});
    } catch (const std::invalid_argument&) {
      exception_thrown = true;
    }

    expect(exception_thrown);
  }

  TEST("Auto-limits: data cube fills the graph bounds") {
    cmp::Plot3D top_view_plot;
    top_view_plot.setBounds(0, 0, 600, 500);
    top_view_plot.setView(0.f, 90.f);
    top_view_plot.plot3({x_data1}, {y_data1}, {z_data1});

    const auto graph_lines =
        getChildComponentHelper<cmp::GraphLine3D>(top_view_plot);
    expectEquals(graph_lines.size(), 1ul);

    const auto& pixel_points = graph_lines[0]->getPixelPoints();
    expectEquals(pixel_points.size(), x_data1.size());

    // In the top view the auto-limits (0..10 in x and y) span the whole
    // graph area: the first point is bottom-left, the last is top-right.
    const auto graph_bounds = graph_lines[0]->getLocalBounds().toFloat();
    expectPointNear(pixel_points[0], {0.f, graph_bounds.getHeight()});
    expectPointNear(pixel_points[1], {graph_bounds.getWidth() / 2.f,
                                      graph_bounds.getHeight() / 2.f});
    expectPointNear(pixel_points[2], {graph_bounds.getWidth(), 0.f});
  }

  TEST("Axes box gets the axes parameters") {
    cmp::Plot3D plot_tmp;
    plot_tmp.setBounds(0, 0, 600, 500);
    plot_tmp.plot3({x_data1}, {y_data1}, {z_data1});

    const auto axes_boxes = getChildComponentHelper<cmp::Axes3DBox>(plot_tmp);
    expectEquals(axes_boxes.size(), 1ul);
    expect(!axes_boxes[0]->getXTicks().empty());
    expect(!axes_boxes[0]->getYTicks().empty());
    expect(!axes_boxes[0]->getZTicks().empty());
    expect(!axes_boxes[0]->getBounds().isEmpty());
  }

  TEST("View control") {
    cmp::Plot3D plot_tmp;

    // MATLAB's default 3D view.
    expectEquals(plot_tmp.getViewAzimuth(), -37.5f);
    expectEquals(plot_tmp.getViewElevation(), 30.f);

    plot_tmp.setView(10.f, 20.f);
    expectEquals(plot_tmp.getViewAzimuth(), 10.f);
    expectEquals(plot_tmp.getViewElevation(), 20.f);
  }

  TEST("Custom look and feel") {
    cmp::Plot3D plot_tmp;
    auto look_and_feel = std::make_unique<cmp::PlotLookAndFeel>();
    plot_tmp.setLookAndFeel(look_and_feel.get());
    expect(&plot_tmp.getLookAndFeel() == look_and_feel.get());
    plot_tmp.setLookAndFeel(nullptr);
  }
}
