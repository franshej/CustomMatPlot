#include <algorithm>

#include "cmp_axes3dbox.h"
#include "cmp_test_helper.hpp"

/* Tests for the 3D axes box.
 *
 * The grid is drawn on the back faces of the data cube (the faces whose
 * outward normal points away from the camera) so it never covers the data,
 * and the tick labels sit on the box edges nearest the viewer.
 */
SECTION(Axes3DBoxTest, "3D axes box") {
  auto containsFace = [](const std::vector<cmp::CubeFace>& faces,
                         const cmp::CubeFace face) {
    return std::find(faces.begin(), faces.end(), face) != faces.end();
  };

  TEST("Back faces: default view") {
    // MATLAB default view(-37.5, 30): looking from the front-left and
    // slightly above, so the right (+x), rear (+y) and bottom (-z) faces
    // are behind the data.
    const auto back_faces = cmp::selectBackFaces(cmp::Camera3D());

    expectEquals(back_faces.size(), std::size_t(3));
    expect(containsFace(back_faces, cmp::CubeFace::x_max));
    expect(containsFace(back_faces, cmp::CubeFace::y_max));
    expect(containsFace(back_faces, cmp::CubeFace::z_min));
  }

  TEST("Back faces: top view") {
    const auto back_faces = cmp::selectBackFaces(cmp::Camera3D(0.f, 90.f));

    expectEquals(back_faces.size(), std::size_t(1));
    expect(containsFace(back_faces, cmp::CubeFace::z_min));
  }

  TEST("Back faces: front view") {
    const auto back_faces = cmp::selectBackFaces(cmp::Camera3D(0.f, 0.f));

    expectEquals(back_faces.size(), std::size_t(1));
    expect(containsFace(back_faces, cmp::CubeFace::y_max));
  }

  TEST("Back faces: view from below") {
    const auto back_faces = cmp::selectBackFaces(cmp::Camera3D(-37.5f, -30.f));

    expect(containsFace(back_faces, cmp::CubeFace::z_max));
    expect(!containsFace(back_faces, cmp::CubeFace::z_min));
  }

  TEST("Tick label edges: default view") {
    const cmp::Camera3D camera;

    // x labels along the front (-y) bottom edge, y labels along the front
    // (-x) bottom edge, on the bottom (-z) face.
    expect(cmp::getXTickLabelYFace(camera) == cmp::CubeFace::y_min);
    expect(cmp::getYTickLabelXFace(camera) == cmp::CubeFace::x_min);
    expect(cmp::getTickLabelZFace(camera) == cmp::CubeFace::z_min);

    // z labels on the front-x/back-y vertical edge (the left edge in the
    // default view, like MATLAB).
    const auto z_edge = cmp::getZTickLabelEdge(camera);
    expect(z_edge.first == cmp::CubeFace::x_min);
    expect(z_edge.second == cmp::CubeFace::y_max);
  }

  TEST("Tick label edges: rotated half a turn") {
    const cmp::Camera3D camera(-37.5f + 180.f, 30.f);

    // The opposite sides face the viewer after a half turn.
    expect(cmp::getXTickLabelYFace(camera) == cmp::CubeFace::y_max);
    expect(cmp::getYTickLabelXFace(camera) == cmp::CubeFace::x_max);
  }

  TEST("Tick label edges: view from below uses the top face") {
    const cmp::Camera3D camera(-37.5f, -30.f);

    expect(cmp::getTickLabelZFace(camera) == cmp::CubeFace::z_max);
  }

  TEST("Auto ticks: linear axes within the limits") {
    cmp::Axes3DBox axes_box;
    axes_box.setParameters({{{0.f, 10.f}, cmp::Scaling::linear},
                            {{-5.f, 5.f}, cmp::Scaling::linear},
                            {{0.f, 100.f}, cmp::Scaling::linear}},
                           cmp::Camera3D());

    expect(!axes_box.getXTicks().empty());
    expect(!axes_box.getYTicks().empty());
    expect(!axes_box.getZTicks().empty());

    for (const auto tick : axes_box.getXTicks()) {
      expect(tick >= 0.f && tick <= 10.f);
    }
    for (const auto tick : axes_box.getYTicks()) {
      expect(tick >= -5.f && tick <= 5.f);
    }
  }

  TEST("Auto ticks: logarithmic axis uses decades") {
    cmp::Axes3DBox axes_box;
    axes_box.setParameters({{{0.f, 10.f}, cmp::Scaling::linear},
                            {{0.f, 10.f}, cmp::Scaling::linear},
                            {{1.f, 1000.f}, cmp::Scaling::logarithmic}},
                           cmp::Camera3D());

    const auto& z_ticks = axes_box.getZTicks();
    expectEquals(z_ticks.size(), std::size_t(4));
    expectEquals(z_ticks[0], 1.f);
    expectEquals(z_ticks[1], 10.f);
    expectEquals(z_ticks[2], 100.f);
    expectEquals(z_ticks[3], 1000.f);
  }

  TEST("Grid lines: linear axis matches the ticks") {
    cmp::Axes3DBox axes_box;
    axes_box.setParameters({{{0.f, 10.f}, cmp::Scaling::linear},
                            {{-5.f, 5.f}, cmp::Scaling::linear},
                            {{0.f, 100.f}, cmp::Scaling::linear}},
                           cmp::Camera3D());

    expectEqualVectors(axes_box.getZGridLines(), axes_box.getZTicks(),
                       [this](auto a, auto b) { expectEquals(a, b); });
  }

  TEST("Grid lines: logarithmic scaling adds minor lines on any axis") {
    const auto log_axis =
        cmp::Axis_f{{1.f, 1000.f}, cmp::Scaling::logarithmic};
    const auto lin_axis = cmp::Axis_f{{0.f, 10.f}, cmp::Scaling::linear};

    // A log axis spanning 1..1000 has the minor lines 1..9, 10..90, 100..900
    // and then 1000: 9 + 9 + 9 + 1 = 28 grid lines with the decade bunching.
    const auto expectLogMinorLines = [this](const std::vector<float>& grid) {
      expectEquals(grid.size(), std::size_t(28));
      expectEquals(grid.front(), 1.f);
      expectEquals(grid.back(), 1000.f);

      const auto contains = [&](const float value) {
        return std::find(grid.begin(), grid.end(), value) != grid.end();
      };
      expect(contains(2.f));
      expect(contains(20.f));
      expect(contains(200.f));
    };

    // Logarithmic scaling must work identically on each of the three axes.
    {
      cmp::Axes3DBox axes_box;
      axes_box.setParameters({log_axis, lin_axis, lin_axis}, cmp::Camera3D());
      expectLogMinorLines(axes_box.getXGridLines());
    }
    {
      cmp::Axes3DBox axes_box;
      axes_box.setParameters({lin_axis, log_axis, lin_axis}, cmp::Camera3D());
      expectLogMinorLines(axes_box.getYGridLines());
    }
    {
      cmp::Axes3DBox axes_box;
      axes_box.setParameters({lin_axis, lin_axis, log_axis}, cmp::Camera3D());
      expectLogMinorLines(axes_box.getZGridLines());
    }
  }

  TEST("No ticks without axis limits") {
    cmp::Axes3DBox axes_box;
    axes_box.setParameters(cmp::Axes3{}, cmp::Camera3D());

    expect(axes_box.getXTicks().empty());
    expect(axes_box.getYTicks().empty());
    expect(axes_box.getZTicks().empty());
  }
}
;
