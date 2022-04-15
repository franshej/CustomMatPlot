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
  /** @brief Calculate x-idxs
   *
   *  Calculate x-idxs based on the x_data, x_lims and the graph_bounds. Makes
   *  sure that there is only one x_data value per x-pixel. If several
   *  data_points share the same pixel value, the first data_point is stored
   *  whereas the following data_points are discarded.
   *
   *  @param common_params common plot parameters.
   *  @param x_data the x_data.
   *  @param x_indx the output x_idxs.
   *  @return void.
   */
  static void calculateXIdxs(const CommonPlotParameterView common_params,
                             const std::vector<FloatType> &x_data,
                             std::vector<std::size_t> &x_idxs);

  /** @brief Calculate xy-idxs
   *
   *  Calculate xy-idxs based on the y_data, y_lims, graph_bounds and the
   *  pre-calculated x_idxs. Downsamples the xy_data without loosing any
   *  resolution in the plot.
   *
   *  @param common_params common plot parameters.
   *  @param x_data the x_data.
   *  @param x_indx the output x_idxs.
   *  @return void.
   */
  static void calculateXYIdxsFrom(const CommonPlotParameterView common_params,
                                  const std::vector<std::size_t> &x_idxs,
                                  const std::vector<FloatType> &y_data,
                                  std::vector<std::size_t> &xy_idxs);
};
}  // namespace cmp