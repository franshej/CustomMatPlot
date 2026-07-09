#include "cmp_math3d.h"

#include "cmp_test_helper.hpp"

SECTION(Math3DTest, "3D math primitives") {
  auto expectNear = [&](const float result, const float expected) {
    expectWithinAbsoluteError(result, expected,
                              1e-5f * (1.0f + std::abs(expected)));
  };

  TEST("Vec3: default construction is zero") {
    constexpr cmp::Vec3f v;

    expectEquals(v.x, 0.f);
    expectEquals(v.y, 0.f);
    expectEquals(v.z, 0.f);
  }

  TEST("Vec3: addition and subtraction") {
    constexpr auto a = cmp::Vec3f{1.f, 2.f, 3.f};
    constexpr auto b = cmp::Vec3f{4.f, 5.f, 6.f};

    expect(a + b == cmp::Vec3f{5.f, 7.f, 9.f});
    expect(b - a == cmp::Vec3f{3.f, 3.f, 3.f});
  }

  TEST("Vec3: scalar multiplication") {
    constexpr auto v = cmp::Vec3f{1.f, -2.f, 3.f};

    expect(v * 2.f == cmp::Vec3f{2.f, -4.f, 6.f});
    expect(v * 0.f == cmp::Vec3f{});
  }

  TEST("Vec3: dot product") {
    constexpr auto a = cmp::Vec3f{1.f, 2.f, 3.f};
    constexpr auto b = cmp::Vec3f{4.f, -5.f, 6.f};

    expectNear(a.dot(b), 12.f);
    expectNear(a.dot(a), 14.f);
  }

  TEST("Vec3: cross product") {
    constexpr auto x = cmp::Vec3f{1.f, 0.f, 0.f};
    constexpr auto y = cmp::Vec3f{0.f, 1.f, 0.f};
    constexpr auto z = cmp::Vec3f{0.f, 0.f, 1.f};

    expect(x.cross(y) == z);
    expect(y.cross(z) == x);
    expect(z.cross(x) == y);
    expect(y.cross(x) == z * -1.f);
  }

  TEST("Vec3: length") {
    expectNear(cmp::Vec3f{3.f, 4.f, 0.f}.length(), 5.f);
    expectNear(cmp::Vec3f{1.f, 2.f, 2.f}.length(), 3.f);
    expectNear(cmp::Vec3f{}.length(), 0.f);
  }

  TEST("Axes3: bundles three axes") {
    const auto axes = cmp::Axes3{{{0.f, 1.f}, cmp::Scaling::linear},
                                 {{0.f, 2.f}, cmp::Scaling::linear},
                                 {{1.f, 1000.f}, cmp::Scaling::logarithmic}};

    expect(axes.x == cmp::Axis_f({0.f, 1.f}, cmp::Scaling::linear));
    expect(axes.y == cmp::Axis_f({0.f, 2.f}, cmp::Scaling::linear));
    expect(axes.z == cmp::Axis_f({1.f, 1000.f}, cmp::Scaling::logarithmic));
  }
};
