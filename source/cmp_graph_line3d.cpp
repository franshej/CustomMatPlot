/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "cmp_graph_line3d.h"

#include <numeric>

#include "cmp_lookandfeel_base.h"
#include "cmp_projector3d.h"

namespace cmp {

void GraphLine3D::setValues(const std::vector<float>& x_data,
                            const std::vector<float>& y_data,
                            const std::vector<float>& z_data) {
  jassert(x_data.size() == y_data.size() && x_data.size() == z_data.size());

  m_x_data = x_data;
  m_y_data = y_data;
  m_z_data = z_data;

  m_pixel_point_indices.resize(m_x_data.size());
  std::iota(m_pixel_point_indices.begin(), m_pixel_point_indices.end(), 0u);
}

const std::vector<float>& GraphLine3D::getXData() const noexcept {
  return m_x_data;
}

const std::vector<float>& GraphLine3D::getYData() const noexcept {
  return m_y_data;
}

const std::vector<float>& GraphLine3D::getZData() const noexcept {
  return m_z_data;
}

void GraphLine3D::setGraphAttribute(const GraphAttribute& graph_attribute) {
  if (graph_attribute.dashed_lengths)
    m_graph_attributes.dashed_lengths = graph_attribute.dashed_lengths;

  if (graph_attribute.graph_colour)
    m_graph_attributes.graph_colour = graph_attribute.graph_colour;

  if (graph_attribute.graph_line_opacity)
    m_graph_attributes.graph_line_opacity = graph_attribute.graph_line_opacity;

  if (graph_attribute.on_pixel_point_paint)
    m_graph_attributes.on_pixel_point_paint =
        graph_attribute.on_pixel_point_paint;

  if (graph_attribute.path_stroke_type)
    m_graph_attributes.path_stroke_type = graph_attribute.path_stroke_type;

  if (graph_attribute.marker)
    m_graph_attributes.marker = graph_attribute.marker;

  if (graph_attribute.gradient_colours)
    m_graph_attributes.gradient_colours = graph_attribute.gradient_colours;
}

const GraphAttribute& GraphLine3D::getGraphAttribute() const noexcept {
  return m_graph_attributes;
}

void GraphLine3D::setColour(const juce::Colour graph_colour) {
  m_graph_attributes.graph_colour = graph_colour;
}

juce::Colour GraphLine3D::getColour() const noexcept {
  return m_graph_attributes.graph_colour.value_or(juce::Colour());
}

void GraphLine3D::updateProjection(const Axes3& axes, const Camera3D& camera) {
  m_axes = axes;
  m_camera = camera;

  updatePixelPointsIntern();
}

const PixelPoints& GraphLine3D::getPixelPoints() const noexcept {
  return m_pixel_points;
}

const std::vector<std::size_t>& GraphLine3D::getPixelPointIndices()
    const noexcept {
  return m_pixel_point_indices;
}

void GraphLine3D::updatePixelPointsIntern() {
  // All three axis limits must be set to project the data.
  if (!m_axes.x.lim || !m_axes.y.lim || !m_axes.z.lim || m_x_data.empty() ||
      getLocalBounds().isEmpty())
    return;

  const Projector3D projector(m_axes, m_camera, getLocalBounds());
  projector.updatePixelPoints(m_x_data, m_y_data, m_z_data, m_pixel_points);
}

void GraphLine3D::resized() { updatePixelPointsIntern(); }

void GraphLine3D::paint(juce::Graphics& g) {
  if (m_lookandfeel && !m_pixel_points.empty()) {
    const GraphLineDataView graph_line_data(m_x_data, m_y_data, m_pixel_points,
                                            m_pixel_point_indices,
                                            m_graph_attributes);

    auto* lnf = static_cast<PlotLookAndFeelBase*>(m_lookandfeel);
    lnf->drawGraphLine(g, graph_line_data, getLocalBounds());
  }
}

void GraphLine3D::lookAndFeelChanged() {
  if (auto* lnf = dynamic_cast<PlotLookAndFeelBase*>(&getLookAndFeel())) {
    m_lookandfeel = lnf;
  } else {
    m_lookandfeel = nullptr;
  }
}

}  // namespace cmp
