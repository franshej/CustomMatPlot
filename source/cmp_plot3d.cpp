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
#include "cmp_graph_line3d.h"
#include "cmp_lookandfeel.h"
#include "cmp_math3d.h"

namespace cmp {

/** The limits spanning all values of all lines. */
static Lim_f findDataLim(const std::vector<std::vector<float>>& data) {
  auto min = std::numeric_limits<float>::max();
  auto max = std::numeric_limits<float>::lowest();

  for (const auto& line_data : data) {
    for (const auto value : line_data) {
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

  // The axes box was added after the lookandfeel was set, so it has not
  // been notified yet.
  resetLookAndFeelChildrens(&getLookAndFeel());
}

void Plot3D::plot3(const std::vector<std::vector<float>>& x_data,
                   const std::vector<std::vector<float>>& y_data,
                   const std::vector<std::vector<float>>& z_data,
                   const GraphAttributeList& graph_attributes) {
  if (x_data.size() != y_data.size() || x_data.size() != z_data.size())
    throw std::invalid_argument(
        "plot3: x_data, y_data and z_data must contain the same number of "
        "lines.");

  auto* lnf = getPlotLookAndFeelBase();

  if (m_graph_lines.size() != x_data.size()) {
    m_graph_lines.resize(x_data.size());

    for (std::size_t i = 0; i < m_graph_lines.size(); ++i) {
      if (m_graph_lines[i]) continue;

      auto graph_line = std::make_unique<GraphLine3D>();

      if (lnf) {
        const auto colour_id = lnf->getColourFromGraphID(i);
        graph_line->setColour(lnf->findAndGetColourFromId(colour_id));
      }

      graph_line->setLookAndFeel(&getLookAndFeel());
      graph_line->setBounds(m_graph_bounds);
      addAndMakeVisible(graph_line.get());

      m_graph_lines[i] = std::move(graph_line);
    }

    m_axes_box->toBack();
  }

  for (std::size_t i = 0; i < m_graph_lines.size(); ++i) {
    if (x_data[i].size() != y_data[i].size() ||
        x_data[i].size() != z_data[i].size())
      throw std::invalid_argument(
          "plot3: the x, y and z values of a line must have the same size.");

    m_graph_lines[i]->setValues(x_data[i], y_data[i], z_data[i]);

    if (i < graph_attributes.size())
      m_graph_lines[i]->setGraphAttribute(graph_attributes[i]);
  }

  if (m_x_autoscale) m_x_axis.lim = findDataLim(x_data);
  if (m_y_autoscale) m_y_axis.lim = findDataLim(y_data);
  if (m_z_autoscale) m_z_axis.lim = findDataLim(z_data);

  updateChildrenParameters();
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

  for (const auto& graph_line : m_graph_lines) {
    if (graph_line) graph_line->updateProjection(axes, camera);
  }

  repaint();
}

juce::Rectangle<int> Plot3D::getGraphBounds3D() noexcept {
  auto* lnf = getPlotLookAndFeelBase();
  if (!lnf) return {};

  // The projected box corners sit on the graph-bound edges, so space is
  // reserved around the graph bounds for the tick labels that are pushed
  // outside the box.
  const auto margin = static_cast<int>(lnf->getMargin());
  const auto label_space =
      2 * margin + static_cast<int>(2.0f * lnf->getGridLabelFont().getHeight());

  return getLocalBounds().reduced(label_space);
}

void Plot3D::resizeChildrens() {
  const auto graph_bounds = getGraphBounds3D();

  if (graph_bounds.isEmpty()) return;

  m_graph_bounds = graph_bounds;
  m_axes_box->setBounds(graph_bounds);

  for (const auto& graph_line : m_graph_lines) {
    if (graph_line) graph_line->setBounds(graph_bounds);
  }
}

void Plot3D::resized() { resizeChildrens(); }

void Plot3D::paint(juce::Graphics& g) {
  if (auto* lnf = getPlotLookAndFeelBase()) {
    lnf->drawBackground(g, m_graph_bounds);
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
