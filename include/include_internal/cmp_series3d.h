/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

/**
 * @file cmp_series3d.h
 *
 * @brief Componenet for drawing 3D series.
 *
 * @ingroup CustomMatPlotInternal
 */

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "cmp_camera3d.h"
#include "cmp_datamodels.h"
#include "cmp_math3d.h"

namespace cmp {

/**
 *  @class Series3D
 *  @brief A class component to draw 3D lines/marker symbols. This is
 *  a subcomponenet to cmp::Plot3D.
 *
 *  The (x, y, z) data is projected into axes-area-local pixel points with
 *  Projector3D and drawn with the same lookandfeel method as the 2D series
 *  lines.
 */
class Series3D : public juce::Component {
 public:
  /** @brief Set the data values for the series.
   *
   *  @param x_data vector of x-values.
   *  @param y_data vector of y-values.
   *  @param z_data vector of z-values.
   *  @return void.
   */
  void setValues(const std::vector<float>& x_data,
                 const std::vector<float>& y_data,
                 const std::vector<float>& z_data);

  /** @brief Get x-values. */
  const std::vector<float>& getXData() const noexcept;

  /** @brief Get y-values. */
  const std::vector<float>& getYData() const noexcept;

  /** @brief Get z-values. */
  const std::vector<float>& getZData() const noexcept;

  /** @brief Set the series attributes. @see SeriesAttribute. */
  void setSeriesAttribute(const SeriesAttribute& series_attribute);

  /** @brief Get the series attributes. @see SeriesAttribute. */
  const SeriesAttribute& getSeriesAttribute() const noexcept;

  /** @brief Set the colour of the series. */
  void setColour(const juce::Colour series_colour);

  /** @brief Get the colour of the series. */
  juce::Colour getColour() const noexcept;

  /** @brief Project the data into pixel points for the given axes and
   *  camera view.
   *
   *  The projection is also recomputed when the component is resized, using
   *  the most recent axes and camera.
   *
   *  @param axes the three plot axes.
   *  @param camera the camera view.
   *  @return void.
   */
  void updateProjection(const Axes3& axes, const Camera3D& camera);

  /** @brief Get the projected pixel points. */
  const PixelPoints& getPixelPoints() const noexcept;

  /** @brief Get the pixel point indices. */
  const std::vector<std::size_t>& getPixelPointIndices() const noexcept;

  //==============================================================================

  /** @internal */
  void resized() override;
  /** @internal */
  void paint(juce::Graphics& g) override;
  /** @internal */
  void lookAndFeelChanged() override;

 private:
  /** @internal */
  void updatePixelPointsIntern();

  std::vector<float> m_x_data, m_y_data, m_z_data;
  PixelPoints m_pixel_points;
  std::vector<std::size_t> m_pixel_point_indices;

  Axes3 m_axes;
  Camera3D m_camera;

  juce::LookAndFeel* m_lookandfeel{nullptr};
  SeriesAttribute m_series_attributes;
};

}  // namespace cmp
