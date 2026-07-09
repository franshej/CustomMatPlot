/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

/**
 * @file cmp_camera3d.h
 *
 * @brief The camera used to view 3D plot data.
 *
 * @ingroup CustomMatPlotInternal
 */

#pragma once

#include <cmath>

#include "cmp_math3d.h"

namespace cmp {

/**
 * @brief An orthographic camera defined by azimuth and elevation, following
 * MATLAB's view(az, el) convention.
 *
 * The azimuth is the horizontal rotation about the z-axis in degrees,
 * measured counter-clockwise from the negative y-axis. The elevation is the
 * angle above the xy-plane in degrees. The default view is MATLAB's default
 * 3D view: view(-37.5, 30).
 *
 * 'toViewSpace' rotates a point into view space, where x points right on the
 * screen, y points up on the screen, and z is the depth towards the camera
 * (a larger z is closer to the viewer).
 */
class Camera3D {
 public:
  /** @brief Construct a camera with MATLAB's default view(-37.5, 30). */
  Camera3D() noexcept { setView(-37.5f, 30.0f); }

  /** @brief Construct a camera with the given azimuth and elevation, like
   * MATLAB's view(az, el). */
  Camera3D(const float azimuth_degrees,
           const float elevation_degrees) noexcept {
    setView(azimuth_degrees, elevation_degrees);
  }

  /** @brief Set the view angles, like MATLAB's view(az, el). */
  void setView(const float azimuth_degrees,
               const float elevation_degrees) noexcept {
    m_azimuth_degrees = azimuth_degrees;
    m_elevation_degrees = elevation_degrees;

    const auto cos_az = std::cos(juce::degreesToRadians(azimuth_degrees));
    const auto sin_az = std::sin(juce::degreesToRadians(azimuth_degrees));
    const auto cos_el = std::cos(juce::degreesToRadians(elevation_degrees));
    const auto sin_el = std::sin(juce::degreesToRadians(elevation_degrees));

    m_screen_x_dir = {cos_az, sin_az, 0.0f};
    m_screen_y_dir = {-sin_el * sin_az, sin_el * cos_az, cos_el};
    m_depth_dir = {cos_el * sin_az, -cos_el * cos_az, sin_el};
  }

  /** @brief Get the azimuth (horizontal rotation about the z-axis) in
   * degrees. */
  float getAzimuthDegrees() const noexcept { return m_azimuth_degrees; }

  /** @brief Get the elevation (angle above the xy-plane) in degrees. */
  float getElevationDegrees() const noexcept { return m_elevation_degrees; }

  /** @brief Rotate a point into view space: x = screen right, y = screen up,
   * z = depth towards the camera. */
  Vec3f toViewSpace(const Vec3f& point) const noexcept {
    return {m_screen_x_dir.dot(point), m_screen_y_dir.dot(point),
            m_depth_dir.dot(point)};
  }

 private:
  float m_azimuth_degrees, m_elevation_degrees;
  Vec3f m_screen_x_dir, m_screen_y_dir, m_depth_dir;
};

}  // namespace cmp
