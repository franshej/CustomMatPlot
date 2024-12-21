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
namespace {
    constexpr size_t MIN_POINTS_FOR_DOWNSAMPLING = 100u;
    constexpr size_t MAX_POINTS_PER_PIXEL = 3u;
    constexpr int PADDING_POINTS = 2;  // Number of points to add outside visible range
    constexpr bool COMPUTE_ALL_POINTS = false;

    template <class FloatType>
    struct MinMaxIndices {
        size_t min_idx;
        size_t max_idx;
        FloatType min_val;
        FloatType max_val;
    };

    template<class FloatType>
    MinMaxIndices<FloatType> findMinMaxIndices(const std::vector<FloatType>& y_data, 
                                   size_t start_idx, 
                                   size_t end_idx) {
        MinMaxIndices<FloatType> result{start_idx, start_idx, y_data[start_idx], y_data[start_idx]};
        
        for (auto idx = start_idx + 1; idx < end_idx; ++idx) {
            auto y_value = y_data[idx];
            if (y_value < result.min_val) {
                result.min_val = y_value;
                result.min_idx = idx;
            } else if (y_value > result.max_val) {
                result.max_val = y_value;
                result.max_idx = idx;
            }
        }
        return result;
    }

    template <class ValueType>
    struct XRangeIndices {
        size_t start_idx;
        size_t end_idx;
    };

    template <class ValueType>
    XRangeIndices<ValueType> findDataRange(const ValueType x_min_lim,
                                          const ValueType x_max_lim,
                                          const std::vector<ValueType>& x_data) {
        if (x_data.empty()) {
            return {0u, 0u};
        }

        // Find start index, ensuring we don't underflow
        const auto raw_start = std::distance(x_data.begin(), 
            std::lower_bound(x_data.begin(), x_data.end(), x_min_lim));
        const auto padded_start = std::max(0L, raw_start - PADDING_POINTS);
        const auto start_idx = static_cast<size_t>(padded_start);

        // Find end index, ensuring we don't overflow
        const auto raw_end = std::distance(x_data.begin(),
            std::upper_bound(x_data.begin(), x_data.end(), x_max_lim));
        const auto max_size = static_cast<long>(x_data.size() - 1);
        const auto padded_end = std::min(max_size, raw_end + PADDING_POINTS);
        const auto end_idx = static_cast<size_t>(padded_end);

        return XRangeIndices<ValueType>{start_idx, end_idx};
    }

    template <class ValueType>
    bool shouldAddPoint(const ValueType current_value, 
                       const ValueType last_value,
                       const ValueType min_distance,
                       const ValueType prev_difference,
                       const ValueType current_difference) {
        // Add point if direction changes or distance exceeds threshold
        const bool direction_changed = (std::signbit(prev_difference) != 
                                      std::signbit(current_difference));
        const bool exceeds_distance = (std::abs(last_value - current_value) > 
                                     min_distance);
        return direction_changed || exceeds_distance;
    }

    template <class FloatType>
    struct PixelColumnData {
        std::vector<size_t> indices;
        FloatType min_val;
        FloatType max_val;
        size_t min_idx;
        size_t max_idx;
    };

    template <class FloatType>
    void processPixelColumn(const std::vector<FloatType>& y_data,
                           size_t start_idx,
                           size_t end_idx,
                           fast_vector<std::size_t>& xy_indices) {
        if (end_idx - start_idx <= MAX_POINTS_PER_PIXEL) {
            // For small segments, include all points
            for (auto i = start_idx; i < end_idx; ++i) {
                xy_indices.push_back(i);
            }
            return;
        }

        // Find min/max points in this column
        auto [min_idx, max_idx, min_val, max_val] = 
            findMinMaxIndices(y_data, start_idx, end_idx);

        // Always include the start point
        xy_indices.push_back(start_idx);

        // Add min/max points in order of appearance
        if (min_idx < max_idx) {
            xy_indices.push_back_if_not_in_back(min_idx);
            xy_indices.push_back_if_not_in_back(max_idx);
        } else {
            xy_indices.push_back_if_not_in_back(max_idx);
            xy_indices.push_back_if_not_in_back(min_idx);
        }

        // Include end point if it's significant
        if (end_idx - 1 != max_idx && end_idx - 1 != min_idx) {
            xy_indices.push_back_if_not_in_back(end_idx - 1);
        }
    }
}

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
static auto calculateXIndicesBetweenStartEnd(
    const ForwardIt start_x_idx, const ForwardIt end_x_idx,
    const Scaling x_scaling, const Lim<ValueType> x_lim, const juce::Rectangle<int> &graph_bounds,
    const std::vector<ValueType>& x_data,
    std::vector<IndexType>& x_based_idxs_out) {
  const auto [x_scale, x_offset] =
      getXScaleAndOffset(float(graph_bounds.getWidth()), x_lim, x_scaling);

  float last_added_x = x_data[start_x_idx];
  std::size_t current_index = start_x_idx;
  std::size_t pixel_point_index{1u};
  const auto inverse_x_scale = 1.0f / x_scale;
  auto last_x_diff = 0.f;

  if (x_scaling == Scaling::linear) {
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
  } else if (x_scaling == Scaling::logarithmic) {
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
void Downsampler<FloatType>::calculateXIndices(
    const Scaling x_scaling, 
    const Lim<FloatType> x_lim, 
    const juce::Rectangle<int>& graph_bounds,
    const std::vector<FloatType>& x_data,
    std::vector<std::size_t>& x_based_idxs_out) 
{
    if (x_data.empty()) {
        x_based_idxs_out.clear();
        return;
    }

    x_based_idxs_out.resize(x_data.size());

    // Handle small datasets without downsampling
    if (x_data.size() < MIN_POINTS_FOR_DOWNSAMPLING) {
        std::iota(x_based_idxs_out.begin(), x_based_idxs_out.end(), 0u);
        return;
    }

    // Find visible data range
    const auto range = findDataRange(x_lim.min, x_lim.max, x_data);
    
    // Set initial point
    x_based_idxs_out[0] = range.start_idx;
    
    // Calculate scale factors
    const auto [scale, offset] = getXScaleAndOffset(
        float(graph_bounds.getWidth()), x_lim, x_scaling);
    const auto inverse_scale = 1.0f / scale;

    size_t output_idx = 1;
    float last_added_x = x_data[range.start_idx];
    float last_diff = 0.f;

    // Process points based on scaling type
    if (x_scaling == Scaling::linear) {
        for (size_t i = range.start_idx + 1; i < range.end_idx; ++i) {
            const auto current_diff = i > 0 ? x_data[i - 1] - x_data[i] : 0.f;
            
            if (shouldAddPoint(x_data[i], last_added_x, inverse_scale, 
                             last_diff, current_diff)) {
                last_added_x = x_data[i];
                x_based_idxs_out[output_idx++] = i;
            }
            
            last_diff = current_diff;
        }
    } else if (x_scaling == Scaling::logarithmic) {
        for (size_t i = range.start_idx + 1; i < range.end_idx; ++i) {
            if (std::log10(std::abs(x_data[i] / last_added_x)) > inverse_scale) {
                last_added_x = x_data[i];
                x_based_idxs_out[output_idx++] = i;
            }
        }
    }

    // Add final point and resize
    x_based_idxs_out[output_idx++] = range.end_idx;
    x_based_idxs_out.resize(output_idx);
}

template <class FloatType>
void Downsampler<FloatType>::calculateXYBasedIdxs(
    const std::vector<std::size_t>& x_indices,
    const std::vector<FloatType>& y_data,
    std::vector<std::size_t>& xy_indices_out) 
{
    if (x_indices.empty()) {
        xy_indices_out.clear();
        return;
    }

    fast_vector<std::size_t> xy_indices(xy_indices_out, y_data.size());

    if (y_data.size() < MIN_POINTS_FOR_DOWNSAMPLING) {
        xy_indices = x_indices;
        xy_indices_out = xy_indices.get();
        return;
    }

    // Process each segment between x-indices
    for (auto i_it = x_indices.begin(); std::next(i_it) != x_indices.end(); ++i_it) {
        const auto start_idx = *i_it;
        const auto end_idx = *std::next(i_it);

        processPixelColumn(y_data, start_idx, end_idx, xy_indices);
    }

    // Ensure the last point is included
    if (!x_indices.empty()) {
        xy_indices.push_back_if_not_in_back(x_indices.back());
    }

    // Transfer results back to output vector
    xy_indices_out = xy_indices.get();
}

template class Downsampler<float>;
}  // namespace cmp
