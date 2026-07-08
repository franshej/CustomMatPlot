/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "cmp_series3d.h"

#include <numeric>

#include "cmp_lookandfeel_base.h"
#include "cmp_projector3d.h"

namespace cmp {

void Series3D::setValues(const std::vector<float>& x_data,
                            const std::vector<float>& y_data,
                            const std::vector<float>& z_data) {
  jassert(x_data.size() == y_data.size() && x_data.size() == z_data.size());

  m_x_data = x_data;
  m_y_data = y_data;
  m_z_data = z_data;

  m_pixel_point_indices.resize(m_x_data.size());
  std::iota(m_pixel_point_indices.begin(), m_pixel_point_indices.end(), 0u);
}

const std::vector<float>& Series3D::getXData() const noexcept {
  return m_x_data;
}

const std::vector<float>& Series3D::getYData() const noexcept {
  return m_y_data;
}

const std::vector<float>& Series3D::getZData() const noexcept {
  return m_z_data;
}

void Series3D::setSeriesAttribute(const SeriesAttribute& series_attribute) {
  if (series_attribute.dashed_lengths)
    m_series_attributes.dashed_lengths = series_attribute.dashed_lengths;

  if (series_attribute.series_colour)
    m_series_attributes.series_colour = series_attribute.series_colour;

  if (series_attribute.series_opacity)
    m_series_attributes.series_opacity = series_attribute.series_opacity;

  if (series_attribute.on_pixel_point_paint)
    m_series_attributes.on_pixel_point_paint =
        series_attribute.on_pixel_point_paint;

  if (series_attribute.path_stroke_type)
    m_series_attributes.path_stroke_type = series_attribute.path_stroke_type;

  if (series_attribute.marker)
    m_series_attributes.marker = series_attribute.marker;

  if (series_attribute.gradient_colours)
    m_series_attributes.gradient_colours = series_attribute.gradient_colours;
}

const SeriesAttribute& Series3D::getSeriesAttribute() const noexcept {
  return m_series_attributes;
}

void Series3D::setColour(const juce::Colour series_colour) {
  m_series_attributes.series_colour = series_colour;
}

juce::Colour Series3D::getColour() const noexcept {
  return m_series_attributes.series_colour.value_or(juce::Colour());
}

void Series3D::updateProjection(const Axes3& axes, const Camera3D& camera) {
  m_axes = axes;
  m_camera = camera;

  updatePixelPointsIntern();
}

const PixelPoints& Series3D::getPixelPoints() const noexcept {
  return m_pixel_points;
}

const std::vector<std::size_t>& Series3D::getPixelPointIndices()
    const noexcept {
  return m_pixel_point_indices;
}

void Series3D::updatePixelPointsIntern() {
  // All three axis limits must be set to project the data.
  if (!m_axes.x.lim || !m_axes.y.lim || !m_axes.z.lim || m_x_data.empty() ||
      getLocalBounds().isEmpty())
    return;

  const Projector3D projector(m_axes, m_camera, getLocalBounds());
  projector.updatePixelPoints(m_x_data, m_y_data, m_z_data, m_pixel_points);
}

void Series3D::resized() { updatePixelPointsIntern(); }

void Series3D::paint(juce::Graphics& g) {
  if (m_lookandfeel && !m_pixel_points.empty()) {
    const SeriesDataView series_data(m_x_data, m_y_data, m_pixel_points,
                                            m_pixel_point_indices,
                                            m_series_attributes);

    auto* lnf = static_cast<PlotLookAndFeelBase*>(m_lookandfeel);
    lnf->drawSeries(g, series_data, getLocalBounds());
  }
}

void Series3D::lookAndFeelChanged() {
  if (auto* lnf = dynamic_cast<PlotLookAndFeelBase*>(&getLookAndFeel())) {
    m_lookandfeel = lnf;
  } else {
    m_lookandfeel = nullptr;
  }
}

}  // namespace cmp
