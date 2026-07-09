/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

/**
 * @file cmp_axis_transform.h
 *
 * @brief The value <-> pixel transform along a single plot axis.
 *
 * @ingroup CustomMatPlotInternal
 */

#pragma once

#include <cmath>

#include "cmp_datamodels.h"

namespace cmp {

/**
 * @brief Maps data values to pixel coordinates (and back) along one axis.
 *
 * The pixel range is explicit: 'pixel_min' is the pixel coordinate of
 * 'axis.lim.min' and 'pixel_max' is the pixel coordinate of 'axis.lim.max'.
 * This makes the coordinate frame explicit at every call site and expresses
 * an inverted axis (like the y-axis, where the minimum value is at the
 * bottom of the axes area) by passing 'pixel_min > pixel_max'.
 */
class AxisTransform {
 public:
  constexpr AxisTransform(const Axis_f axis, const float pixel_min,
                          const float pixel_max) noexcept
      : m_axis{axis},
        m_pixel_min{pixel_min},
        m_scale{axis.scaling == Scaling::linear
                    ? (pixel_max - pixel_min) / (axis.lim.max - axis.lim.min)
                    : (pixel_max - pixel_min) /
                          std::log10(axis.lim.max / axis.lim.min)} {}

  /** @brief Map a data value to its pixel coordinate. */
  constexpr float toPixel(const float value) const noexcept {
    switch (m_axis.scaling) {
      case Scaling::logarithmic:
        return m_pixel_min + std::log10(value / m_axis.lim.min) * m_scale;
      case Scaling::linear:
      default:
        return m_pixel_min + (value - m_axis.lim.min) * m_scale;
    }
  }

  /** @brief Map a pixel coordinate back to its data value. */
  constexpr float fromPixel(const float pixel) const noexcept {
    switch (m_axis.scaling) {
      case Scaling::logarithmic:
        return m_axis.lim.min *
               std::pow(10.0f, (pixel - m_pixel_min) / m_scale);
      case Scaling::linear:
      default:
        return m_axis.lim.min + (pixel - m_pixel_min) / m_scale;
    }
  }

 private:
  Axis_f m_axis;
  float m_pixel_min;
  float m_scale;
};

}  // namespace cmp
