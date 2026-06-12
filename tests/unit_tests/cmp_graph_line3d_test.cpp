#include <vector>

#include "cmp_graph_line3d.h"
#include "cmp_lookandfeel.h"
#include "cmp_projector3d.h"
#include "cmp_test_helper.hpp"

SECTION(GraphLine3DTest, "3D graph line") {
  auto expectPointNear = [&](const juce::Point<float> result,
                             const juce::Point<float> expected) {
    expectWithinAbsoluteError(result.getX(), expected.getX(), 1e-2f);
    expectWithinAbsoluteError(result.getY(), expected.getY(), 1e-2f);
  };

  const std::vector<float> x_data = {0.f, 5.f, 10.f};
  const std::vector<float> y_data = {0.f, 5.f, 10.f};
  const std::vector<float> z_data = {10.f, 5.f, 0.f};

  const auto axes = cmp::Axes3{{{0.f, 10.f}, cmp::Scaling::linear},
                               {{0.f, 10.f}, cmp::Scaling::linear},
                               {{0.f, 10.f}, cmp::Scaling::linear}};

  TEST("Data storage") {
    cmp::GraphLine3D graph_line;
    graph_line.setValues(x_data, y_data, z_data);

    expectEquals(graph_line.getXData().size(), x_data.size());
    expectEquals(graph_line.getXData()[1], 5.f);
    expectEquals(graph_line.getYData()[2], 10.f);
    expectEquals(graph_line.getZData()[0], 10.f);

    const auto& indices = graph_line.getPixelPointIndices();
    expectEquals(indices.size(), x_data.size());
    expectEquals(indices[0], std::size_t(0));
    expectEquals(indices[2], std::size_t(2));
  }

  TEST("Projection matches Projector3D") {
    cmp::GraphLine3D graph_line;
    graph_line.setBounds(0, 0, 500, 400);
    graph_line.setValues(x_data, y_data, z_data);

    const cmp::Camera3D top_view(0.f, 90.f);
    graph_line.updateProjection(axes, top_view);

    const auto& pixel_points = graph_line.getPixelPoints();
    expectEquals(pixel_points.size(), x_data.size());

    const cmp::Projector3D projector(axes, top_view,
                                     graph_line.getLocalBounds());
    for (std::size_t i = 0; i < pixel_points.size(); ++i) {
      expectPointNear(pixel_points[i],
                      projector.toPixel({x_data[i], y_data[i], z_data[i]}));
    }

    // Top view: x across the width, y up the height.
    expectPointNear(pixel_points[0], {0.f, 400.f});
    expectPointNear(pixel_points[1], {250.f, 200.f});
    expectPointNear(pixel_points[2], {500.f, 0.f});
  }

  TEST("Resizing recomputes the projection") {
    cmp::GraphLine3D graph_line;
    graph_line.setBounds(0, 0, 500, 400);
    graph_line.setValues(x_data, y_data, z_data);
    graph_line.updateProjection(axes, cmp::Camera3D(0.f, 90.f));

    graph_line.setBounds(0, 0, 250, 200);

    expectPointNear(graph_line.getPixelPoints()[2], {250.f, 0.f});
  }

  TEST("No projection without axis limits") {
    cmp::GraphLine3D graph_line;
    graph_line.setBounds(0, 0, 500, 400);
    graph_line.setValues(x_data, y_data, z_data);

    graph_line.updateProjection(cmp::Axes3{}, cmp::Camera3D());

    expect(graph_line.getPixelPoints().empty());
  }

  TEST("Graph attributes and colour") {
    cmp::GraphLine3D graph_line;
    graph_line.setColour(juce::Colours::red);

    expect(graph_line.getColour() == juce::Colours::red);

    auto attribute = cmp::GraphAttribute();
    attribute.graph_line_opacity = 0.5f;
    graph_line.setGraphAttribute(attribute);

    expectEquals(*graph_line.getGraphAttribute().graph_line_opacity, 0.5f);
    // Setting one attribute does not reset the colour.
    expect(graph_line.getColour() == juce::Colours::red);
  }
}
;
