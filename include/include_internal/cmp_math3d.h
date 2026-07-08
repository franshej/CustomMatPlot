/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

/**
 * @file cmp_math3d.h
 *
 * @brief 3D math primitives used by the 3D plotting pipeline.
 *
 * @ingroup CustomMatPlotInternal
 */

#pragma once

#include <cmath>

#include "cmp_datamodels.h"

namespace cmp {

/** @brief A template struct that defines a 3D point/vector. */
template <class ValueType>
struct Vec3 {
  ValueType x{}, y{}, z{};

  /** @brief Component-wise addition. */
  constexpr Vec3 operator+(const Vec3& rhs) const noexcept {
    return {x + rhs.x, y + rhs.y, z + rhs.z};
  }

  /** @brief Component-wise subtraction. */
  constexpr Vec3 operator-(const Vec3& rhs) const noexcept {
    return {x - rhs.x, y - rhs.y, z - rhs.z};
  }

  /** @brief Scale every component by a scalar. */
  constexpr Vec3 operator*(const ValueType scalar) const noexcept {
    return {x * scalar, y * scalar, z * scalar};
  }

  /** @brief Component-wise equality. */
  constexpr bool operator==(const Vec3& rhs) const noexcept {
    return x == rhs.x && y == rhs.y && z == rhs.z;
  }

  /** @brief Component-wise inequality. */
  constexpr bool operator!=(const Vec3& rhs) const noexcept {
    return !(*this == rhs);
  }

  /** @brief The dot (scalar) product with another vector. */
  constexpr ValueType dot(const Vec3& rhs) const noexcept {
    return x * rhs.x + y * rhs.y + z * rhs.z;
  }

  /** @brief The cross (vector) product with another vector. */
  constexpr Vec3 cross(const Vec3& rhs) const noexcept {
    return {y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z,
            x * rhs.y - y * rhs.x};
  }

  /** @brief The Euclidean length (magnitude) of the vector. */
  ValueType length() const noexcept { return std::sqrt(dot(*this)); }
};

/** @brief A 3D point/vector using float. */
typedef Vec3<float> Vec3f;

/** @brief The three axes of a 3D plot. */
struct Axes3 {
  Axis_f x, y, z;
};

}  // namespace cmp
