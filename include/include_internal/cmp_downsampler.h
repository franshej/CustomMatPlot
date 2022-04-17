/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

/*
 * @file cmp_downsampler.h
 *
 * @brief Class to downsample data before its being plotted to save cpu.
 *
 * @ingroup CustomMatPlotInternal
 *
 * @author Frans Rosencrantz
 * Contact: Frans.Rosencrantz@gmail.com
 *
 */

#pragma once

#include <cmp_datamodels.h>

namespace cmp {

/**
 * \class Downsampler
 * \brief A class for downsampler xy-data
 *
 * The idea is to use this class to downsample data before it's being plotted.
 * We don't want to plot data-points that share the same pixel.
 */
template <class FloatType>
class Downsampler {
 public:
  /** @brief Calculate x-based downsample indices
   *
   * Calculate ds indices based on the x_data, x_lims and the graph_bounds. The
   * output indices is used to get the x/y-data to only plot one value per
   * x-pixel. If several x/y-points share the same x-pixel value, the first
   * index is stored whereas the following indices are discarded until next
   * x-pixel is reached.
   *
   *  @param common_params common plot parameters.
   *  @param x_data the x_data to be plotted.
   *  @param x_based_ds_idxs the output x-indices.
   *  @return void.
   */
  static void calculateXBasedDSIdxs(const CommonPlotParameterView common_params,
                                    const std::vector<FloatType> &x_data,
                                    std::vector<std::size_t> &x_idxs);

  /** @brief Calculate xy-indices
   *
   * Calculate the indices that can be used to get the data_points to be
   * plotted without loosing any resolution in the plot. Calculate xy-indices
   * based on the y_data, y_lims, graph_bounds and the pre-calculated x-indices
   * @see calculateXIdxs.
   *
   *  @param common_params common plot parameters.
   *  @param x_idxs the x-indices calculated in @see CalculateXIdxs.
   *  @param y_data the y_data to be plotted.
   *  @param xy_based_ds_idxs indices used to downsample the data.
   *  @return void.
   */
  static void calculateXYBasedDSIdxs(
      const CommonPlotParameterView common_params,
      const std::vector<std::size_t> &x_idxs,
      const std::vector<FloatType> &y_data, std::vector<std::size_t> &xy_idxs);
};
}  // namespace cmp