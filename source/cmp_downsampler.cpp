/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "cmp_downsampler.h"

#include <algorithm>
#include <cstddef>

#include "cmp_datamodels.h"
#include "cmp_utils.h"

namespace cmp {
template <class ValueType>
static auto computeXStartIdx(const ValueType x_min_lim,
                             const std::vector<ValueType>& x_data) {
  int start_x_index{0};

  // The points that are on top of each other or outside the graph area are
  // dicarded.
  for (const auto& x : x_data) {
    if (x >= x_min_lim) {
      int start_i_outside = 2;
      while (true) {
        if ((start_x_index - start_i_outside) >= 0) {
          // start_i_outside indices outside the left side to get rid of
          // artifacts.
          start_x_index = start_x_index - start_i_outside;
          break;
        }
        --start_i_outside;
      }
      break;
    }
    start_x_index++;
  }

  start_x_index =
      start_x_index == x_data.size() ? x_data.size() - 1u : start_x_index;

  return start_x_index;
}

template <class ValueType>
static auto computeXEndIdx(const ValueType x_max_lim,
                           const std::vector<ValueType>& x_data) {
  int end_x_index{int(x_data.size() - 1)};

  for (auto x = x_data.rbegin(); x != x_data.rend(); ++x) {
    if (*x <= x_max_lim || end_x_index == 0u) {
      auto start_i_outside = 2u;
      while (true) {
        if (end_x_index < x_data.size() - start_i_outside) {
          // Two indices outside the right side to get rid of artifacts.
          end_x_index = end_x_index + start_i_outside;
          break;
        }
        --start_i_outside;
      }
      break;
    }
    end_x_index--;
  }

  return end_x_index;
}

template <class ValueType, class IndexType, class ForwardIt>
static auto calculateXIdxsBetweenStartEnd(
    const ForwardIt start_x_idx, const ForwardIt end_x_idx,
    const cmp::CommonPlotParameterView common_params,
    const std::vector<ValueType>& x_data,
    std::vector<IndexType>& x_based_idxs_out) {
  const auto [x_scale, x_offset] =
      getXScaleAndOffset(float(common_params.graph_bounds.getWidth()),
                         common_params.x_lim, common_params.x_scaling);

  float last_added_x = x_data[start_x_idx];
  std::size_t current_index = start_x_idx;
  std::size_t pixel_point_index{1u};
  const auto inverse_x_scale = 1.0f / x_scale;
  auto last_x_diff = 0.f;

  if (common_params.x_scaling == Scaling::linear) {
    for (auto i = current_index; i < end_x_idx; ++i) {
      const auto current_x_diff = i > 0 ? x_data[i - 1] - x_data[i] : 0.f;

      // If the x values suddenly go opposite, we need to add that value.
      const auto force_add_x =
          (std::signbit(last_x_diff) != std::signbit(current_x_diff));
      last_x_diff = current_x_diff;

      if (force_add_x || (std::abs(last_added_x - x_data[i])) > inverse_x_scale) {
        last_added_x = x_data[i];
        x_based_idxs_out[pixel_point_index++] = i;
      }
    }
  } else if (common_params.x_scaling == Scaling::logarithmic) {
    for (auto x = x_data.begin() + start_x_idx; x != x_data.begin() + end_x_idx;
         ++x) {
      if (std::log10(std::abs(*x / last_added_x)) > inverse_x_scale) {
        last_added_x = *x;
        x_based_idxs_out[pixel_point_index++] = current_index;
      }
      current_index++;
    }
  }

  const auto x_idxs_size_required = pixel_point_index + 1;
  return x_idxs_size_required;
}

template <class FloatType>
void Downsampler<FloatType>::calculateXBasedDSIdxs(
    const CommonPlotParameterView common_params,
    const std::vector<FloatType>& x_data,
    std::vector<std::size_t>& x_based_idxs_out) {
  x_based_idxs_out.resize(x_data.size());

  const auto& bounds = common_params.graph_bounds;
  const auto& x_lim = common_params.x_lim;

  std::size_t max_x_index{x_data.size() - 1u};

  // If the graph has less than 100 values we simply plot all points even if
  // they are on top of each other.
  if (x_data.size() < 100u) {
    std::iota(x_based_idxs_out.begin(), x_based_idxs_out.end(), 0u);
    return;
  }

  constexpr auto compute_all_points = false;
  // If the graph is zoomed in we only plots some points.
  const auto start_x_index =
      compute_all_points
          ? size_t(0u)
          : computeXStartIdx<decltype(x_lim.min)>(x_lim.min, x_data);
  const auto end_x_index =
      compute_all_points
          ? x_data.size() - 1u
          : computeXEndIdx<decltype(x_lim.max)>(x_lim.max, x_data);

  x_based_idxs_out.front() = start_x_index;

  const auto x_idxs_size_required =
      calculateXIdxsBetweenStartEnd<FloatType, size_t>(
          start_x_index, end_x_index, common_params, x_data, x_based_idxs_out);

  x_based_idxs_out.resize(x_idxs_size_required);
  x_based_idxs_out.back() = end_x_index;
}

template <class FloatType>
void Downsampler<FloatType>::calculateXYBasedIdxs(
    const CommonPlotParameterView common_params,
    const std::vector<std::size_t>& x_based_idxs_out,
    const std::vector<FloatType>& y_data,
    std::vector<std::size_t>& xy_based_idxs_out) {
  if (x_based_idxs_out.empty()) return;

  // Using a fast_vector to avoid heap allocations and range checks.
  fast_vector<std::size_t> xy_based_idxs(xy_based_idxs_out, y_data.size());

  // If the graph has less than 100 values we simply plot all points.
  if (xy_based_idxs.get().size() < 100u) {
    xy_based_idxs = x_based_idxs_out;
    return;
  }

  for (auto i_it = x_based_idxs_out.begin();; ++i_it) {
    // We have reached the last x-based ds index. Let's add it.
    UNLIKELY if (std::next(i_it) == x_based_idxs_out.end()) {
      xy_based_idxs.push_back(*i_it);
      break;
    }

    // Calculated how many data values that share the same pixel coloumn.
    const auto num_data_sharing_same_pixel_col = *std::next(i_it) - *i_it;

    if (num_data_sharing_same_pixel_col <= 3u) {
      for (auto i = 0u; i < num_data_sharing_same_pixel_col; ++i) {
        xy_based_idxs.push_back(*std::next(i_it, i));
      }
      // Let's find the min and max y-value that share the same x-pixel and get
      // their indices.
    } else if (num_data_sharing_same_pixel_col > 3u) {
      auto current_index = *i_it + 1;

      auto max_val = y_data[current_index];
      auto min_val = max_val;
      auto max_idx = current_index;
      auto min_idx = current_index;
      auto min_last = true;

      for (; current_index < *std::next(i_it); ++current_index) {
        auto y = y_data[current_index];

        if (y < min_val) {
          min_val = y;
          min_idx = current_index;
          min_last = true;
        } else if (y > max_val) {
          max_val = y;
          max_idx = current_index;
          min_last = false;
        }
      }

      // First add the first index we found for this pixel column.
      xy_based_idxs.push_back(*i_it);

      // Then add the mix and max indices for the pixel column.
      if (min_last) {
        xy_based_idxs.push_back_if_not_in_back(max_idx);
        xy_based_idxs.push_back_if_not_in_back(min_idx);
      } else {
        xy_based_idxs.push_back_if_not_in_back(min_idx);
        xy_based_idxs.push_back_if_not_in_back(max_idx);
      }

      // Finally add the last index we found for this pixel column.
      xy_based_idxs.push_back_if_not_in_back(current_index);
    }
  }
}

template class Downsampler<float>;
}  // namespace cmp
