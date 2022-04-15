/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "cmp_downsampler.h"

#include "cmp_utils.h"

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
                                          const auto& x_data, auto& x_idx) {
  const auto [x_scale, x_offset] =
      getXScaleAndOffset(float(common_params.graph_bounds.getWidth()),
                         common_params.x_lim, common_params.x_scaling);

  float last_added_x = std::numeric_limits<float>::min();
  std::size_t current_index = start_x_idx;
  std::size_t graph_point_index{0u};
  const auto inverse_x_scale = 1.f / x_scale;

  if (common_params.x_scaling == Scaling::linear) {
    for (auto x = x_data.begin() + start_x_idx; x != x_data.begin() + end_x_idx;
         ++x) {
      if (abs(*x - last_added_x) > inverse_x_scale) {
        last_added_x = *x;
        x_idx[graph_point_index++] = current_index;
      }
      current_index++;
    }
  } else if (common_params.x_scaling == Scaling::logarithmic) {
    for (auto x = x_data.begin() + start_x_idx; x != x_data.begin() + end_x_idx;
         ++x) {
      if (log10(abs(*x / last_added_x)) > inverse_x_scale) {
        last_added_x = *x;
        x_idx[graph_point_index++] = current_index;
      }
      current_index++;
    }
  }

  const auto x_idxs_size_required = graph_point_index + 1;
  return x_idxs_size_required;
}

template <class FloatType>
void Downsampler<FloatType>::calculateXIdxs(
    const CommonPlotParameterView common_params,
    const std::vector<FloatType>& x_data, std::vector<std::size_t>& x_idxs) {
  x_idxs.resize(x_data.size());

  const auto& bounds = common_params.graph_bounds;
  const auto& x_lim = common_params.x_lim;

  std::size_t max_x_index{x_data.size() - 1u};

  // If the graph has less than 10 values we simply plot all points even if
  // they are on top of each other.
  if (x_data.size() < 10u) {
    std::iota(x_idxs.begin(), x_idxs.end(), 0u);
    return;
  }

  const auto start_x_index = computeXStartIdx(x_lim.min, x_data);

  const auto end_x_index = computeXEndIdx(x_lim.max, x_data);

  x_idxs.front() = start_x_index;

  const auto x_idxs_size_required = calculateXIdxsBetweenStartEnd(
      start_x_index, end_x_index, common_params, x_data, x_idxs);

  x_idxs.resize(x_idxs_size_required);

  x_idxs.back() = end_x_index;
}

template <class FloatType>
void Downsampler<FloatType>::calculateXYIdxsFrom(
    const CommonPlotParameterView common_params,
    const std::vector<std::size_t>& x_idxs,
    const std::vector<FloatType>& y_data, std::vector<std::size_t>& xy_idxs) {
  if (x_idxs.empty()) return;

  xy_idxs.resize(y_data.size());

  if (xy_idxs.size() < 10u) {
    std::iota(xy_idxs.begin(), xy_idxs.end(), 0u);
    return;
  }

  auto xy_i = 0u;
  auto xy_size = 0u;

  for (auto i = x_idxs.begin();; ++i) {
    if (i + 1 == x_idxs.end()) {
      xy_idxs[xy_i++] = *i;

      xy_size += 1;

      break;
    }

    xy_idxs[xy_i++] = *i;

    const auto index_diff = *(i + 1) - *i;

    if (index_diff == 2u) {
      xy_idxs[xy_i++] = *i + 1;

      xy_size += 2;
    } else if (index_diff == 3u) {
      xy_idxs[xy_i++] = *i + 1;
      xy_idxs[xy_i++] = *i + 2;

      xy_size += 3;
    } else if (index_diff > 3u) {
      auto j = *i + 1;

      auto max_val = y_data[j];
      auto min_val = y_data[j];
      auto max_idx = j;
      auto min_idx = j;

      for (; j < *(i + 1); ++j) {
        auto y = y_data[j];

        if (y < min_val) {
          min_val = y;

          min_idx = j;
        } else if (y > max_val) {
          max_val = y;

          max_idx = j;
        }
      }

      xy_idxs[xy_i++] = min_idx;
      xy_idxs[xy_i++] = max_idx;
      xy_size += 3;
    } else {
      xy_size += 1;
    }
  }

  xy_idxs.resize(xy_size);
}

template class Downsampler<float>;
}  // namespace cmp
