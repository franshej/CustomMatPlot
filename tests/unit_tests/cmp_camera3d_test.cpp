#include "cmp_camera3d.h"

#include "cmp_test_helper.hpp"

/* Tests for the 3D camera.
 *
 * The view convention follows MATLAB's view(az, el): azimuth rotates about
 * the z-axis counter-clockwise from the negative y-axis, elevation is the
 * angle above the xy-plane. View space is x = screen right, y = screen up,
 * z = depth towards the camera.
 */
SECTION(Camera3DTest, "3D camera") {
  auto expectNear = [&](const float result, const float expected) {
    expectWithinAbsoluteError(result, expected, 1e-4f);
  };

  auto expectVecNear = [&](const cmp::Vec3f result, const cmp::Vec3f expected) {
    expectWithinAbsoluteError(result.x, expected.x, 1e-4f);
    expectWithinAbsoluteError(result.y, expected.y, 1e-4f);
    expectWithinAbsoluteError(result.z, expected.z, 1e-4f);
  };

  TEST("Front view: view(0, 0) shows the xz-plane") {
    const cmp::Camera3D camera(0.f, 0.f);

    // Screen x = data x, screen y = data z, depth = -data y (the camera
    // looks from the negative y-axis towards positive y).
    expectVecNear(camera.toViewSpace({1.f, 2.f, 3.f}), {1.f, 3.f, -2.f});
  }

  TEST("Side view: view(90, 0) shows the yz-plane") {
    const cmp::Camera3D camera(90.f, 0.f);

    expectVecNear(camera.toViewSpace({1.f, 2.f, 3.f}), {2.f, 3.f, 1.f});
  }

  TEST("Top view: view(0, 90) shows the xy-plane") {
    const cmp::Camera3D camera(0.f, 90.f);

    expectVecNear(camera.toViewSpace({1.f, 2.f, 3.f}), {1.f, 2.f, 3.f});
  }

  TEST("Default view is MATLAB's view(-37.5, 30)") {
    const cmp::Camera3D camera;

    expectNear(camera.getAzimuthDegrees(), -37.5f);
    expectNear(camera.getElevationDegrees(), 30.f);

    // Where the unit-cube corner (1, 1, 1) lands, computed from
    // viewmtx(-37.5, 30).
    expectVecNear(camera.toViewSpace({1.f, 1.f, 1.f}),
                  {0.18459f, 1.56708f, -0.71427f});
    expectVecNear(camera.toViewSpace({0.f, 0.f, 0.f}), {0.f, 0.f, 0.f});
  }

  TEST("View transform is a rotation: lengths are preserved") {
    for (const auto camera : {cmp::Camera3D(), cmp::Camera3D(123.f, -45.f),
                              cmp::Camera3D(0.f, 0.f)}) {
      const auto point = cmp::Vec3f{1.f, 2.f, 3.f};

      expectNear(camera.toViewSpace(point).length(), point.length());
    }
  }

  TEST("Azimuth is periodic: view(az, el) == view(az + 360, el)") {
    const cmp::Camera3D camera_a(-37.5f, 30.f);
    const cmp::Camera3D camera_b(-37.5f + 360.f, 30.f);
    const auto point = cmp::Vec3f{1.f, 2.f, 3.f};

    expectVecNear(camera_a.toViewSpace(point), camera_b.toViewSpace(point));
  }

  TEST("setView updates the view") {
    cmp::Camera3D camera(0.f, 0.f);
    camera.setView(0.f, 90.f);

    expectNear(camera.getElevationDegrees(), 90.f);
    expectVecNear(camera.toViewSpace({1.f, 2.f, 3.f}), {1.f, 2.f, 3.f});
  }
};
