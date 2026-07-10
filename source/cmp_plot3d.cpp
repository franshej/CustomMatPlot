/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "cmp_plot3d.h"

#include <limits>
#include <stdexcept>

#include "cmp_axes3dbox.h"
#include "cmp_camera3d.h"
#include "cmp_lookandfeel.h"
#include "cmp_math3d.h"
#include "cmp_plot.h"
#include "cmp_projector3d.h"
#include "cmp_series3d.h"

namespace cmp {

/** The limits spanning one axis (x, y or z) across all series. */
static Lim_f findSeriesLim(const std::vector<Series3DData>& series,
                           std::vector<float> Series3DData::*axis) {
  auto min = std::numeric_limits<float>::max();
  auto max = std::numeric_limits<float>::lowest();

  for (const auto& s : series) {
    for (const auto value : s.*axis) {
      min = std::min(min, value);
      max = std::max(max, value);
    }
  }

  if (min > max) return {0.0f, 0.0f};

  // A flat line still needs a non-empty range to span a cube face.
  if (min == max) return {min - 1.0f, max + 1.0f};

  return {min, max};
}

Plot3D::~Plot3D() { setLookAndFeel(nullptr); }

Plot3D::Plot3D(const Scaling x_scaling, const Scaling y_scaling,
               const Scaling z_scaling)
    : m_x_axis{{}, x_scaling},
      m_y_axis{{}, y_scaling},
      m_z_axis{{}, z_scaling},
      // MATLAB's default 3D view.
      m_azimuth_degrees{-37.5f},
      m_elevation_degrees{30.0f},
      m_axes_box{std::make_unique<Axes3DBox>()} {
  m_lookandfeel_default = std::make_unique<PlotLookAndFeel>();
  setLookAndFeel(m_lookandfeel_default.get());

  addAndMakeVisible(m_axes_box.get());
  m_axes_box->toBack();
  // The tick labels are drawn pushed outside the cube (and thus outside the
  // box's own bounds), so painting must not be clipped to those bounds.
  m_axes_box->setPaintingIsUnclipped(true);

  for (auto* label : {&m_x_label, &m_y_label, &m_z_label, &m_title_label}) {
    label->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);
  }

  // The children were added after the lookandfeel was set, so they have
  // not been notified yet.
  resetLookAndFeelChildrens(&getLookAndFeel());
}

void Plot3D::xLim(const float min, const float max) {
  if (min >= max)
    throw std::invalid_argument("Min value must be lower than max value.");

  m_x_axis.lim = {min, max};
  m_x_autoscale = false;

  updateChildrenParameters();
}

void Plot3D::yLim(const float min, const float max) {
  if (min >= max)
    throw std::invalid_argument("Min value must be lower than max value.");

  m_y_axis.lim = {min, max};
  m_y_autoscale = false;

  updateChildrenParameters();
}

void Plot3D::zLim(const float min, const float max) {
  if (min >= max)
    throw std::invalid_argument("Min value must be lower than max value.");

  m_z_axis.lim = {min, max};
  m_z_autoscale = false;

  updateChildrenParameters();
}

void Plot3D::setXLabel(const std::string& x_label) {
  m_x_label.setText(x_label, juce::NotificationType::dontSendNotification);

  updateLabelsIntern();
}

void Plot3D::setYLabel(const std::string& y_label) {
  m_y_label.setText(y_label, juce::NotificationType::dontSendNotification);

  updateLabelsIntern();
}

void Plot3D::setZLabel(const std::string& z_label) {
  m_z_label.setText(z_label, juce::NotificationType::dontSendNotification);

  updateLabelsIntern();
}

void Plot3D::setTitle(const std::string& title) {
  m_title_label.setText(title, juce::NotificationType::dontSendNotification);

  updateLabelsIntern();
}

const juce::Label& Plot3D::getXLabel() const noexcept { return m_x_label; }

const juce::Label& Plot3D::getYLabel() const noexcept { return m_y_label; }

const juce::Label& Plot3D::getZLabel() const noexcept { return m_z_label; }

const juce::Label& Plot3D::getTitleLabel() const noexcept {
  return m_title_label;
}

void Plot3D::plot3(const std::vector<Series3DData>& series) {
  auto* lnf = getPlotLookAndFeelBase();

  if (m_series.size() != series.size()) {
    m_series.resize(series.size());

    for (std::size_t i = 0; i < m_series.size(); ++i) {
      if (m_series[i]) continue;

      auto s = std::make_unique<Series3D>();

      if (lnf) {
        const auto colour_id = lnf->getColourFromSeriesID(i);
        s->setColour(lnf->findAndGetColourFromId(colour_id));
      }

      s->setLookAndFeel(&getLookAndFeel());
      s->setBounds(m_axes_bounds);
      addAndMakeVisible(s.get());

      m_series[i] = std::move(s);
    }

    m_axes_box->toBack();
  }

  for (const auto& s : series)
    if (s.x.size() != s.y.size() || s.x.size() != s.z.size())
      throw std::invalid_argument(
          "plot3: the x, y and z values of a series must have the same size.");

  if (m_x_autoscale) m_x_axis.lim = findSeriesLim(series, &Series3DData::x);
  if (m_y_autoscale) m_y_axis.lim = findSeriesLim(series, &Series3DData::y);
  if (m_z_autoscale) m_z_axis.lim = findSeriesLim(series, &Series3DData::z);

  // Copy each series' data straight into its component: one copy of the
  // caller's data, with no intermediate parallel arrays.
  for (std::size_t i = 0; i < series.size(); ++i) {
    m_series[i]->setValues(series[i].x, series[i].y, series[i].z);
    m_series[i]->setSeriesAttribute(series[i].attribute);
  }

  updateChildrenParameters();
}

void Plot3D::plot3(const Series3DData& series) {
  plot3(std::vector<Series3DData>{series});
}

void Plot3D::setView(const float azimuth_degrees,
                     const float elevation_degrees) {
  m_azimuth_degrees = azimuth_degrees;
  m_elevation_degrees = elevation_degrees;

  updateChildrenParameters();
}

float Plot3D::getViewAzimuth() const noexcept { return m_azimuth_degrees; }

float Plot3D::getViewElevation() const noexcept { return m_elevation_degrees; }

void Plot3D::updateChildrenParameters() {
  const auto axes = Axes3{m_x_axis, m_y_axis, m_z_axis};
  const auto camera = Camera3D(m_azimuth_degrees, m_elevation_degrees);

  m_axes_box->setParameters(axes, camera);

  for (const auto& series : m_series) {
    if (series) series->updateProjection(axes, camera);
  }

  repaint();
}

juce::Rectangle<int> Plot3D::getAxesBounds3D() noexcept {
  auto* lnf = getPlotLookAndFeelBase();
  if (!lnf) return {};

  // The projected box corners sit on the series-bound edges, so space is
  // reserved around the axes bounds for the tick labels pushed outside the
  // box and, beyond them, the x/y/z axis labels.
  const auto margin = static_cast<int>(lnf->getMargin());
  const auto tick_label_space =
      static_cast<int>(2.0f * lnf->getGridLabelFont().getHeight());
  const auto axis_label_space =
      static_cast<int>(lnf->getXYTitleFont().getHeight());
  const auto label_space = 2 * margin + tick_label_space + axis_label_space;

  return getLocalBounds().reduced(label_space);
}

void Plot3D::resizeChildrens() {
  const auto axes_bounds = getAxesBounds3D();

  if (axes_bounds.isEmpty()) return;

  m_axes_bounds = axes_bounds;
  m_axes_box->setBounds(axes_bounds);

  for (const auto& series : m_series) {
    if (series) series->setBounds(axes_bounds);
  }

  updateLabelsIntern();
}

void Plot3D::updateLabelsIntern() {
  auto* lnf = getPlotLookAndFeelBase();
  if (!lnf || getLocalBounds().isEmpty()) return;

  const auto font = lnf->getXYTitleFont();
  const auto label_height = static_cast<int>(font.getHeight()) + 2;
  const auto margin = static_cast<int>(lnf->getMarginSmall());
  const auto bounds = getLocalBounds();

  // The title stays centred along the top of the plot.
  m_title_label.setFont(font);
  m_title_label.setColour(juce::Label::textColourId,
                          findColour(Plot::title_label_colour));
  m_title_label.setBounds(0, margin, bounds.getWidth(), label_height);

  if (m_axes_bounds.isEmpty()) return;

  // The x/y/z labels are placed at the projected midpoint of their axis and
  // pushed outward from the box centre, past the tick labels, so each label
  // sits beside the axis it names in any view (like the tick labels do).
  const auto axes = Axes3{m_x_axis, m_y_axis, m_z_axis};
  const auto camera = Camera3D(m_azimuth_degrees, m_elevation_degrees);
  const Projector3D projector(axes, camera, m_axes_bounds);

  const auto origin = m_axes_bounds.getPosition().toFloat();

  const auto& xl = m_x_axis.lim;
  const auto& yl = m_y_axis.lim;
  const auto& zl = m_z_axis.lim;

  const auto box_centre = (projector.toPixel({xl.min, yl.min, zl.min}) +
                           projector.toPixel({xl.max, yl.max, zl.max})) /
                          2.0f;

  // Push just past the tick labels so each axis label sits beside its axis
  // without floating off to the plot edge.
  const auto label_margin_px =
      static_cast<float>(lnf->getAxisLabelDistanceFromAxesBound(label_height));

  const auto placeAxisLabel = [&](juce::Label& label, const int colour_id,
                                  const Vec3f& edge_a, const Vec3f& edge_b) {
    label.setFont(font);
    label.setColour(juce::Label::textColourId, findColour(colour_id));

    const auto edge_mid =
        (projector.toPixel(edge_a) + projector.toPixel(edge_b)) / 2.0f;

    auto direction = edge_mid - box_centre;
    if (direction.getDistanceFromOrigin() > 0.0f)
      direction /= direction.getDistanceFromOrigin();

    const auto centre = origin + edge_mid + direction * label_margin_px;
    const auto width = font.getStringWidth(label.getText()) + 2 * margin;

    label.setBounds(
        juce::Rectangle<int>(width, label_height).withCentre(centre.toInt()));
  };

  // The faces the labels run along, matching the tick-label placement.
  const auto x_yface = getFaceValue(getXTickLabelYFace(camera), axes);
  const auto y_xface = getFaceValue(getYTickLabelXFace(camera), axes);
  const auto xy_zface = getFaceValue(getTickLabelZFace(camera), axes);
  const auto z_edge = getZTickLabelEdge(camera);
  const auto z_xface = getFaceValue(z_edge.first, axes);
  const auto z_yface = getFaceValue(z_edge.second, axes);

  placeAxisLabel(m_x_label, Plot::x_label_colour, {xl.min, x_yface, xy_zface},
                 {xl.max, x_yface, xy_zface});
  placeAxisLabel(m_y_label, Plot::y_label_colour, {y_xface, yl.min, xy_zface},
                 {y_xface, yl.max, xy_zface});
  placeAxisLabel(m_z_label, Plot::y_label_colour, {z_xface, z_yface, zl.min},
                 {z_xface, z_yface, zl.max});
}

void Plot3D::resized() { resizeChildrens(); }

void Plot3D::paint(juce::Graphics& g) {
  if (auto* lnf = getPlotLookAndFeelBase()) {
    lnf->drawBackground(g, m_axes_bounds);
  }
}

void Plot3D::parentHierarchyChanged() { lookAndFeelChanged(); }

void Plot3D::lookAndFeelChanged() {
  if (auto* lnf = dynamic_cast<PlotLookAndFeelBase*>(&getLookAndFeel())) {
    resetLookAndFeelChildrens(lnf);
    resizeChildrens();
  } else {
    resetLookAndFeelChildrens(nullptr);
  }
}

void Plot3D::resetLookAndFeelChildrens(juce::LookAndFeel* lookandfeel) {
  for (auto* child : getChildren()) {
    child->setLookAndFeel(lookandfeel);
  }
}

PlotLookAndFeelBase* Plot3D::getPlotLookAndFeelBase() {
  return dynamic_cast<PlotLookAndFeelBase*>(&getLookAndFeel());
}

}  // namespace cmp
