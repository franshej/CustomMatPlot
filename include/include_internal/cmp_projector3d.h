/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

/**
 * @file cmp_projector3d.h
 *
 * @brief Projects 3D data points into 2D pixel points.
 *
 * @ingroup CustomMatPlotInternal
 */

#pragma once

#include <cmath>
#include <utility>
#include <vector>

#include "cmp_axis_transform.h"
#include "cmp_camera3d.h"
#include "cmp_math3d.h"

namespace cmp {

/**
 * @brief Projects (x, y, z) data points into graph-area-local pixel points.
 *
 * The data cube spanned by the three axis limits is normalized into a unit
 * cube centered on the origin (logarithmic axes are normalized in log
 * space), rotated into view space by the camera, and fitted into the graph
 * bounds: the view-space bounding box of the data cube is stretched to fill
 * the graph area, like the 2D pipeline stretches the x/y limits and like
 * MATLAB's default 'stretch' camera mode.
 *
 * Pixel points are graph-area-local, matching the 2D pixel-point pipeline,
 * so everything downstream of pixel points can be reused.
 */
class Projector3D {
 public:
  Projector3D(const Axes3& axes, const Camera3D& camera,
              const juce::Rectangle<int>& graph_bounds) noexcept
      : Projector3D(axes, camera, graph_bounds, viewSpaceBoundingBox(camera)) {
  }

  /** @brief Project a single data point to its graph-area-local pixel
   * point. */
  juce::Point<float> toPixel(const Vec3f& data_point) const noexcept {
    const auto view_point = m_camera.toViewSpace(toCenteredUnitCube(data_point));

    return {m_screen_x.toPixel(view_point.x), m_screen_y.toPixel(view_point.y)};
  }

  /** @brief Project the given data to pixel points. The pixel-point vector
   * is resized to the data size. */
  void updatePixelPoints(const std::vector<float>& x_data,
                         const std::vector<float>& y_data,
                         const std::vector<float>& z_data,
                         PixelPoints& pixel_points) const {
    // The x/y/z data must be equal length; the caller (the public plot3 API)
    // guarantees this. There is a bug in the code if this assert happens.
    jassert(x_data.size() == y_data.size() &&
            y_data.size() == z_data.size());

    pixel_points.resize(x_data.size());

    for (std::size_t i = 0; i < x_data.size(); ++i) {
      pixel_points[i] = toPixel({x_data[i], y_data[i], z_data[i]});
    }
  }

 private:
  Projector3D(const Axes3& axes, const Camera3D& camera,
              const juce::Rectangle<int>& graph_bounds,
              const std::pair<Lim_f, Lim_f>& view_bounding_box) noexcept
      : m_axes{axes},
        m_camera{camera},
        m_screen_x{{view_bounding_box.first, Scaling::linear}, 0.0f,
                   static_cast<float>(graph_bounds.getWidth())},
        // The y-axis is inverted: the bottom of the view-space bounding box
        // maps to the graph-area height.
        m_screen_y{{view_bounding_box.second, Scaling::linear},
                   static_cast<float>(graph_bounds.getHeight()), 0.0f} {}

  /** Normalize a value into [0, 1] along one axis. */
  static float toUnitRange(const float value, const Axis_f& axis) noexcept {
    if (axis.scaling == Scaling::logarithmic) {
      return std::log10(value / axis.lim.min) /
             std::log10(axis.lim.max / axis.lim.min);
    }

    return (value - axis.lim.min) / (axis.lim.max - axis.lim.min);
  }

  /** Normalize a data point into the unit cube centered on the origin. */
  Vec3f toCenteredUnitCube(const Vec3f& data_point) const noexcept {
    return {toUnitRange(data_point.x, m_axes.x) - 0.5f,
            toUnitRange(data_point.y, m_axes.y) - 0.5f,
            toUnitRange(data_point.z, m_axes.z) - 0.5f};
  }

  /** The view-space x/y bounding box of the centered unit cube. */
  static std::pair<Lim_f, Lim_f> viewSpaceBoundingBox(
      const Camera3D& camera) noexcept {
    auto x_lim = Lim_f(0.0f, 0.0f);
    auto y_lim = Lim_f(0.0f, 0.0f);

    for (const auto x : {-0.5f, 0.5f}) {
      for (const auto y : {-0.5f, 0.5f}) {
        for (const auto z : {-0.5f, 0.5f}) {
          const auto corner = camera.toViewSpace({x, y, z});

          x_lim.min = std::min(x_lim.min, corner.x);
          x_lim.max = std::max(x_lim.max, corner.x);
          y_lim.min = std::min(y_lim.min, corner.y);
          y_lim.max = std::max(y_lim.max, corner.y);
        }
      }
    }

    return {x_lim, y_lim};
  }

  Axes3 m_axes;
  Camera3D m_camera;
  AxisTransform m_screen_x, m_screen_y;
};

}  // namespace cmp
