/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "cmp_axes3dbox.h"

#include <cmath>

#include "cmp_lookandfeel_base.h"
#include "cmp_plot.h"
#include "cmp_projector3d.h"
#include "cmp_utils.h"

namespace cmp {

static constexpr int num_ticks_per_axis = 5;

/** Auto-generate the ticks within the limits of one axis. */
static std::vector<float> generateAxisTicks(const Axis_f& axis) {
  if (!axis.lim) return {};

  auto ticks = std::vector<float>();

  if (axis.scaling == Scaling::logarithmic) {
    // One tick per decade.
    const auto first_decade = std::ceil(std::log10(axis.lim.min));
    const auto last_decade = std::floor(std::log10(axis.lim.max));

    for (auto decade = first_decade; decade <= last_decade; ++decade) {
      ticks.push_back(std::pow(10.0f, decade));
    }

    return ticks;
  }

  for (const auto tick : TicksGenerator::generateTicks(
           axis.lim.min, axis.lim.max, num_ticks_per_axis, {})) {
    if (tick >= axis.lim.min && tick <= axis.lim.max) ticks.push_back(tick);
  }

  return ticks;
}

/** Grid-line positions for one axis. Linear axes reuse the label ticks; log
 * axes add the minor lines (2..9 within every decade) so the grid shows the
 * characteristic logarithmic bunching, not just one evenly-spaced line per
 * decade. */
static std::vector<float> generateAxisGridLines(const Axis_f& axis) {
  if (!axis.lim) return {};

  if (axis.scaling != Scaling::logarithmic) return generateAxisTicks(axis);

  auto lines = std::vector<float>();

  const auto first_decade = std::floor(std::log10(axis.lim.min));
  const auto last_decade = std::floor(std::log10(axis.lim.max));

  for (auto decade = first_decade; decade <= last_decade; ++decade) {
    const auto decade_value = std::pow(10.0f, decade);
    for (int n = 1; n <= 9; ++n) {
      const auto value = static_cast<float>(n) * decade_value;
      if (value >= axis.lim.min && value <= axis.lim.max) lines.push_back(value);
    }
  }

  return lines;
}

void Axes3DBox::setParameters(const Axes3& axes, const Camera3D& camera) {
  m_axes = axes;
  m_camera = camera;

  updateTicks();
  repaint();
}

const std::vector<float>& Axes3DBox::getXTicks() const noexcept {
  return m_x_ticks;
}

const std::vector<float>& Axes3DBox::getYTicks() const noexcept {
  return m_y_ticks;
}

const std::vector<float>& Axes3DBox::getZTicks() const noexcept {
  return m_z_ticks;
}

const std::vector<float>& Axes3DBox::getXGridLines() const noexcept {
  return m_x_grid;
}

const std::vector<float>& Axes3DBox::getYGridLines() const noexcept {
  return m_y_grid;
}

const std::vector<float>& Axes3DBox::getZGridLines() const noexcept {
  return m_z_grid;
}

void Axes3DBox::updateTicks() {
  m_x_ticks = generateAxisTicks(m_axes.x);
  m_y_ticks = generateAxisTicks(m_axes.y);
  m_z_ticks = generateAxisTicks(m_axes.z);

  m_x_grid = generateAxisGridLines(m_axes.x);
  m_y_grid = generateAxisGridLines(m_axes.y);
  m_z_grid = generateAxisGridLines(m_axes.z);
}

void Axes3DBox::resized() {}

void Axes3DBox::paint(juce::Graphics& g) {
  if (!m_lookandfeel || !m_axes.x.lim || !m_axes.y.lim || !m_axes.z.lim ||
      getLocalBounds().isEmpty())
    return;

  const Projector3D projector(m_axes, m_camera, getLocalBounds());
  auto* lnf = static_cast<PlotLookAndFeelBase*>(m_lookandfeel);

  const auto& x_lim = m_axes.x.lim;
  const auto& y_lim = m_axes.y.lim;
  const auto& z_lim = m_axes.z.lim;

  const auto drawProjectedLine = [&](const Vec3f& from, const Vec3f& to) {
    const auto from_pixel = projector.toPixel(from);
    const auto to_pixel = projector.toPixel(to);

    g.drawLine(from_pixel.getX(), from_pixel.getY(), to_pixel.getX(),
               to_pixel.getY(), 1.0f);
  };

  g.setColour(findColour(Plot::grid_colour));

  for (const auto face : selectBackFaces(m_camera)) {
    const auto face_value = getFaceValue(face, m_axes);

    switch (face) {
      case CubeFace::x_min:
      case CubeFace::x_max:
        // The face spans the y- and z-axes.
        for (const auto tick : m_y_grid)
          drawProjectedLine({face_value, tick, z_lim.min},
                            {face_value, tick, z_lim.max});
        for (const auto tick : m_z_grid)
          drawProjectedLine({face_value, y_lim.min, tick},
                            {face_value, y_lim.max, tick});

        drawProjectedLine({face_value, y_lim.min, z_lim.min},
                          {face_value, y_lim.max, z_lim.min});
        drawProjectedLine({face_value, y_lim.max, z_lim.min},
                          {face_value, y_lim.max, z_lim.max});
        drawProjectedLine({face_value, y_lim.max, z_lim.max},
                          {face_value, y_lim.min, z_lim.max});
        drawProjectedLine({face_value, y_lim.min, z_lim.max},
                          {face_value, y_lim.min, z_lim.min});
        break;

      case CubeFace::y_min:
      case CubeFace::y_max:
        // The face spans the x- and z-axes.
        for (const auto tick : m_x_grid)
          drawProjectedLine({tick, face_value, z_lim.min},
                            {tick, face_value, z_lim.max});
        for (const auto tick : m_z_grid)
          drawProjectedLine({x_lim.min, face_value, tick},
                            {x_lim.max, face_value, tick});

        drawProjectedLine({x_lim.min, face_value, z_lim.min},
                          {x_lim.max, face_value, z_lim.min});
        drawProjectedLine({x_lim.max, face_value, z_lim.min},
                          {x_lim.max, face_value, z_lim.max});
        drawProjectedLine({x_lim.max, face_value, z_lim.max},
                          {x_lim.min, face_value, z_lim.max});
        drawProjectedLine({x_lim.min, face_value, z_lim.max},
                          {x_lim.min, face_value, z_lim.min});
        break;

      case CubeFace::z_min:
      case CubeFace::z_max:
      default:
        // The face spans the x- and y-axes.
        for (const auto tick : m_x_grid)
          drawProjectedLine({tick, y_lim.min, face_value},
                            {tick, y_lim.max, face_value});
        for (const auto tick : m_y_grid)
          drawProjectedLine({x_lim.min, tick, face_value},
                            {x_lim.max, tick, face_value});

        drawProjectedLine({x_lim.min, y_lim.min, face_value},
                          {x_lim.max, y_lim.min, face_value});
        drawProjectedLine({x_lim.max, y_lim.min, face_value},
                          {x_lim.max, y_lim.max, face_value});
        drawProjectedLine({x_lim.max, y_lim.max, face_value},
                          {x_lim.min, y_lim.max, face_value});
        drawProjectedLine({x_lim.min, y_lim.max, face_value},
                          {x_lim.min, y_lim.min, face_value});
        break;
    }
  }

  // Tick labels are pushed outwards from the centre of the box so they end
  // up outside the box edge they belong to in any view.
  const auto centre_pixel =
      (projector.toPixel({x_lim.min, y_lim.min, z_lim.min}) +
       projector.toPixel({x_lim.max, y_lim.max, z_lim.max})) /
      2.0f;

  const auto tick_label_margin_px =
      static_cast<float>(lnf->getGridLabelDistanceFromGraphBound());

  const auto drawTickLabel = [&](const float tick, const Vec3f& data_point) {
    const auto pixel = projector.toPixel(data_point);
    auto direction = pixel - centre_pixel;

    if (direction.getDistanceFromOrigin() > 0.0f)
      direction /= direction.getDistanceFromOrigin();

    const auto label_centre = pixel + direction * tick_label_margin_px;
    const auto label_text = valueToStringWithoutTrailingZeros(tick);

    g.drawText(label_text,
               juce::Rectangle<float>(60.0f, 20.0f).withCentre(label_centre),
               juce::Justification::centred);
  };

  g.setFont(lnf->getGridLabelFont());

  const auto x_label_y_value = getFaceValue(getXTickLabelYFace(m_camera), m_axes);
  const auto y_label_x_value = getFaceValue(getYTickLabelXFace(m_camera), m_axes);
  const auto xy_label_z_value = getFaceValue(getTickLabelZFace(m_camera), m_axes);
  const auto z_label_edge = getZTickLabelEdge(m_camera);
  const auto z_label_x_value = getFaceValue(z_label_edge.first, m_axes);
  const auto z_label_y_value = getFaceValue(z_label_edge.second, m_axes);

  g.setColour(findColour(Plot::x_grid_label_colour));
  for (const auto tick : m_x_ticks)
    drawTickLabel(tick, {tick, x_label_y_value, xy_label_z_value});

  g.setColour(findColour(Plot::y_grid_label_colour));
  for (const auto tick : m_y_ticks)
    drawTickLabel(tick, {y_label_x_value, tick, xy_label_z_value});

  g.setColour(findColour(Plot::y_grid_label_colour));
  for (const auto tick : m_z_ticks)
    drawTickLabel(tick, {z_label_x_value, z_label_y_value, tick});
}

void Axes3DBox::lookAndFeelChanged() {
  if (auto* lnf = dynamic_cast<PlotLookAndFeelBase*>(&getLookAndFeel())) {
    m_lookandfeel = lnf;
  } else {
    m_lookandfeel = nullptr;
  }
}

}  // namespace cmp
