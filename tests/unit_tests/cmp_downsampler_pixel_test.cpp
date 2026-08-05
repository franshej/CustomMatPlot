#include <vector>

#include "cmp_downsampler.h"
#include "cmp_test_helper.hpp"

/* Tests for the screen-space downsampler used by the 3D series.
 *
 * It keeps a point whenever it lands on a different pixel than the last kept
 * point, and always keeps the first and last point. It makes no assumption
 * that the points are sorted, so it also suits projected 3D curves that
 * double back on themselves.
 */
SECTION(DownsamplerPixelTest, "Pixel-based downsampler") {
  // Downsampling only kicks in above this many points; below it every index
  // is kept.
  constexpr std::size_t min_points = 100u;

  const auto makeIndices = [](const cmp::PixelPoints& points) {
    std::vector<std::size_t> indices;
    cmp::Downsampler<float>::calculatePixelBasedIdxs(points, indices);
    return indices;
  };

  TEST("Empty input gives no indices") {
    expect(makeIndices({}).empty());
  }

  TEST("Small inputs are kept in full") {
    cmp::PixelPoints points(min_points - 1u, {0.0f, 0.0f});

    const auto indices = makeIndices(points);

    expectEquals(indices.size(), points.size());
    for (std::size_t i = 0; i < indices.size(); ++i) expectEquals(indices[i], i);
  }

  TEST("Points sharing a pixel collapse to the first and last") {
    // Every point rounds to pixel (0, 0), so only the endpoints survive.
    const cmp::PixelPoints points(4u * min_points, {0.2f, -0.3f});

    const auto indices = makeIndices(points);

    expectEquals(indices.size(), std::size_t(2u));
    expectEquals(indices.front(), std::size_t(0u));
    expectEquals(indices.back(), points.size() - 1u);
  }

  TEST("Points on distinct pixels are all kept") {
    cmp::PixelPoints points;
    for (std::size_t i = 0; i < 4u * min_points; ++i)
      points.push_back({float(i), float(i)});

    const auto indices = makeIndices(points);

    expectEquals(indices.size(), points.size());
    for (std::size_t i = 0; i < indices.size(); ++i) expectEquals(indices[i], i);
  }

  TEST("Indices are strictly increasing and in range") {
    // A self-crossing curve: the pixel sequence is not sorted on either axis.
    cmp::PixelPoints points;
    for (std::size_t i = 0; i < 10u * min_points; ++i) {
      const auto t = float(i) * 0.05f;
      points.push_back({50.0f * std::cos(t), 50.0f * std::sin(t)});
    }

    const auto indices = makeIndices(points);

    expect(!indices.empty());
    expectEquals(indices.front(), std::size_t(0u));
    expectEquals(indices.back(), points.size() - 1u);

    for (std::size_t i = 1; i < indices.size(); ++i) {
      expect(indices[i] > indices[i - 1], "indices must strictly increase");
      expect(indices[i] < points.size(), "indices must stay in range");
    }
  }

  TEST("A revisited pixel is kept when it is not consecutive") {
    // Out and back: the curve returns to pixel 0, which must still be kept,
    // because the points in between left that pixel. Only the duplicated
    // turnaround point shares a pixel with its predecessor.
    constexpr std::size_t leg = 2u * min_points;

    cmp::PixelPoints points;
    for (std::size_t i = 0; i < leg; ++i) points.push_back({float(i), 0.0f});
    for (std::size_t i = leg; i-- > 0;) points.push_back({float(i), 0.0f});

    const auto indices = makeIndices(points);

    // Every point is kept except the repeated turnaround.
    expectEquals(indices.size(), points.size() - 1u);

    // The curve ends back on pixel 0, and that return is still drawn.
    expectEquals(indices.back(), points.size() - 1u);
    expectEquals(points[indices.back()].getX(), 0.0f);
  }
}
