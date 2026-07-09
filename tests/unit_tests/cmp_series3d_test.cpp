#include <vector>

#include "cmp_series3d.h"
#include "cmp_lookandfeel.h"
#include "cmp_projector3d.h"
#include "cmp_test_helper.hpp"

SECTION(Series3DTest, "3D series") {
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
    cmp::Series3D series;
    series.setValues(x_data, y_data, z_data);

    expectEquals(series.getXData().size(), x_data.size());
    expectEquals(series.getXData()[1], 5.f);
    expectEquals(series.getYData()[2], 10.f);
    expectEquals(series.getZData()[0], 10.f);

    const auto& indices = series.getPixelPointIndices();
    expectEquals(indices.size(), x_data.size());
    expectEquals(indices[0], std::size_t(0));
    expectEquals(indices[2], std::size_t(2));
  }

  TEST("Projection matches Projector3D") {
    cmp::Series3D series;
    series.setBounds(0, 0, 500, 400);
    series.setValues(x_data, y_data, z_data);

    const cmp::Camera3D top_view(0.f, 90.f);
    series.updateProjection(axes, top_view);

    const auto& pixel_points = series.getPixelPoints();
    expectEquals(pixel_points.size(), x_data.size());

    const cmp::Projector3D projector(axes, top_view,
                                     series.getLocalBounds());
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
    cmp::Series3D series;
    series.setBounds(0, 0, 500, 400);
    series.setValues(x_data, y_data, z_data);
    series.updateProjection(axes, cmp::Camera3D(0.f, 90.f));

    series.setBounds(0, 0, 250, 200);

    expectPointNear(series.getPixelPoints()[2], {250.f, 0.f});
  }

  TEST("No projection without axis limits") {
    cmp::Series3D series;
    series.setBounds(0, 0, 500, 400);
    series.setValues(x_data, y_data, z_data);

    series.updateProjection(cmp::Axes3{}, cmp::Camera3D());

    expect(series.getPixelPoints().empty());
  }

  TEST("Series attributes and colour") {
    cmp::Series3D series;
    series.setColour(juce::Colours::red);

    expect(series.getColour() == juce::Colours::red);

    auto attribute = cmp::SeriesAttribute();
    attribute.series_opacity = 0.5f;
    series.setSeriesAttribute(attribute);

    expectEquals(*series.getSeriesAttribute().series_opacity, 0.5f);
    // Setting one attribute does not reset the colour.
    expect(series.getColour() == juce::Colours::red);
  }
}
;
