/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "cmp_downsampler.h"

#include "cmp_datamodels.h"
#include "cmp_utils.h"
#include <algorithm>

namespace cmp {
static auto computeXStartIdx(const auto x_min_lim, const auto& x_data) {
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

static auto computeXEndIdx(const auto x_max_lim, const auto& x_data) {
  std::size_t end_x_index{x_data.size() - 1};

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

static auto calculateXIdxsBetweenStartEnd(const auto start_x_idx,
                                          const auto end_x_idx,
                                          const auto common_params,
                                          const auto& x_data, auto& x_based_ds_idxs) {
  const auto [x_scale, x_offset] =
      getXScaleAndOffset(float(common_params.graph_bounds.getWidth()),
                         common_params.x_lim, common_params.x_scaling);

  float last_added_x = x_data[start_x_idx];
  std::size_t current_index = start_x_idx;
  std::size_t graph_point_index{1u};
  const auto inverse_x_scale = 1.0f / x_scale;
  auto last_x_diff = 0.f;

  if (common_params.x_scaling == Scaling::linear) {
    for (auto i = current_index; i < end_x_idx;
         ++i) {
      const auto current_x_diff = i > 0 ? x_data[i - 1] - x_data[i] : 0.f;

      // If the x values suddenly go opposite, we need to add that value.
      const auto force_add_x = (std::signbit(last_x_diff) != std::signbit(current_x_diff));
      last_x_diff = current_x_diff;

      if (force_add_x || (abs(last_added_x - x_data[i])) > inverse_x_scale) {
        last_added_x = x_data[i];
        x_based_ds_idxs[graph_point_index++] = i;
      }
    }
  } else if (common_params.x_scaling == Scaling::logarithmic) {
    for (auto x = x_data.begin() + start_x_idx; x != x_data.begin() + end_x_idx;
         ++x) {
      if (log10(abs(*x / last_added_x)) > inverse_x_scale) {
        last_added_x = *x;
        x_based_ds_idxs[graph_point_index++] = current_index;
      }
      current_index++;
    }
  }

  const auto x_idxs_size_required = graph_point_index + 1;
  return x_idxs_size_required;
}

template <class FloatType>
void Downsampler<FloatType>::calculateXBasedDSIdxs(
    const CommonPlotParameterView common_params,
    const std::vector<FloatType>& x_data,
    std::vector<std::size_t>& x_based_ds_idxs) {
  x_based_ds_idxs.resize(x_data.size());

  const auto& bounds = common_params.graph_bounds;
  const auto& x_lim = common_params.x_lim;

  std::size_t max_x_index{x_data.size() - 1u};

  // If the graph has less than 10 values we simply plot all points even if
  // they are on top of each other.
  if (x_data.size() < 10u) {
    std::iota(x_based_ds_idxs.begin(), x_based_ds_idxs.end(), 0u);
    return;
  }

  const auto start_x_index = computeXStartIdx(x_lim.min, x_data);

  const auto end_x_index = computeXEndIdx(x_lim.max, x_data);

  x_based_ds_idxs.front() = start_x_index;

  const auto x_idxs_size_required = calculateXIdxsBetweenStartEnd(
      start_x_index, end_x_index, common_params, x_data, x_based_ds_idxs);

  x_based_ds_idxs.resize(x_idxs_size_required);

  x_based_ds_idxs.back() = end_x_index;
}

template <class FloatType>
void Downsampler<FloatType>::calculateXYBasedDSIdxs(
    const CommonPlotParameterView common_params,
    const std::vector<std::size_t>& x_based_ds_idxs,
    const std::vector<FloatType>& y_data,
    std::vector<std::size_t>& xy_based_ds_idxs) {
  if (x_based_ds_idxs.empty()) return;

  xy_based_ds_idxs.resize(y_data.size());

  // If the graph has less than 10 values we simply plot all points.
  if (xy_based_ds_idxs.size() < 10u) {
    xy_based_ds_idxs = x_based_ds_idxs;
  }

  auto xy_i = 0u;
  auto xy_size = 0u;

  for (auto i = x_based_ds_idxs.begin();; ++i) {
    // We have reached the last x-based ds index. Let's add it.
    if (i + 1 == x_based_ds_idxs.end()) UNLIKELY {
        xy_based_ds_idxs[xy_i++] = *i;

        xy_size += 1;

        break;
      }

    // Add the current x-based ds index.
    xy_based_ds_idxs[xy_i++] = *i;

    // Calculated the index diff between the current and the next x-based ds
    // index.
    const auto index_diff = *(i + 1) - *i;

    if (index_diff == 2u) {
      xy_based_ds_idxs[xy_i++] = *i + 1;

      xy_size += 2;
    } else if (index_diff == 3u) {
      xy_based_ds_idxs[xy_i++] = *i + 1;
      xy_based_ds_idxs[xy_i++] = *i + 2;

      xy_size += 3;

      // Let's find the min and max y-value that share the same x-pixel and get
      // their indices.
    } else if (index_diff > 3u) {
      auto j = *i + 1;

      auto max_val = y_data[j];
      auto min_val = y_data[j];
      auto max_idx = j;
      auto min_idx = j;
      auto min_last = true;

      for (; j < *(i + 1); ++j) {
        auto y = y_data[j];

        if (y < min_val) {
          min_val = y;

          min_idx = j;

          min_last = true;
        } else if (y > max_val) {
          max_val = y;

          max_idx = j;

          min_last = false;
        }
      }

      // Make sure that we att the indices in the correct order.
      if (min_last) {
        xy_based_ds_idxs[xy_i++] = max_idx;
        xy_based_ds_idxs[xy_i++] = min_idx;
      } else {
        xy_based_ds_idxs[xy_i++] = min_idx;
        xy_based_ds_idxs[xy_i++] = max_idx;
      }

      xy_size += 3;
    } else {
      xy_size += 1;
    }
  }

  xy_based_ds_idxs.resize(xy_size);
}

template class Downsampler<float>;
}  // namespace cmp
