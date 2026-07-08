/**
 * @file cmp_plot3d.h
 * @brief Component for plotting 3-D lines
 * @ingroup CustomMatPlot
 * @details This file contains the Plot3D class for creating 3D line plots
 *          with a MATLAB-style orthographic view.
 * @author Frans Rosencrantz
 * @contact Frans.Rosencrantz@gmail.com
 */

#pragma once

#include <memory>

#include "cmp_datamodels.h"
#include "cmp_lookandfeel_base.h"

namespace cmp {

class GraphLine3D;
class Axes3DBox;

/**
 * @class Plot3D
 * @brief A component to plot 3-D lines
 * @details A sibling component to cmp::Plot for 3D data. The data is
 *          projected with a software orthographic camera (MATLAB-style
 *          default view) into the same pixel-point pipeline as the 2D plot.
 *          Features:
 *          - 3D line plots with auto-limits
 *          - Linear and logarithmic axis scaling per axis
 *          - Camera view control (azimuth/elevation)
 * @see Plot
 */
class Plot3D : public juce::Component {
public:
  /** Destructor, making sure to set the lookandfeel in all subcomponenets to
   * nullptr. */
  ~Plot3D() override;

  /**
   * @brief Construct a 3-D plot with the given per-axis scaling.
   * @param x_scaling linear or logarithmic scaling for the x-axis
   * @param y_scaling linear or logarithmic scaling for the y-axis
   * @param z_scaling linear or logarithmic scaling for the z-axis
   */
  Plot3D(const Scaling x_scaling = Scaling::linear,
         const Scaling y_scaling = Scaling::linear,
         const Scaling z_scaling = Scaling::linear);

  /**
   * @brief Plot 3-D line data
   * @details Each entry in the outer vectors describes one graph line. The
   *          x/y/z vectors of a line must have the same size. The axis
   *          limits are auto-scaled to the data unless they have been set
   *          explicitly.
   * @param x_data x-values of the lines
   * @param y_data y-values of the lines
   * @param z_data z-values of the lines
   * @param graph_attributes optional attributes per line
   */
  void plot3(const std::vector<std::vector<float>> &x_data,
             const std::vector<std::vector<float>> &y_data,
             const std::vector<std::vector<float>> &z_data,
             const GraphAttributeList &graph_attributes = {});

  /**
   * @brief Set the X-limits
   * @details Disables x-axis auto-scaling.
   * @param min minimum value
   * @param max maximum value
   */
  void xLim(const float min, const float max);

  /**
   * @brief Set the Y-limits
   * @details Disables y-axis auto-scaling.
   * @param min minimum value
   * @param max maximum value
   */
  void yLim(const float min, const float max);

  /**
   * @brief Set the Z-limits
   * @details Disables z-axis auto-scaling.
   * @param min minimum value
   * @param max maximum value
   */
  void zLim(const float min, const float max);

  /** @brief Set the text on the x-axis. */
  void setXLabel(const std::string &x_label);

  /** @brief Set the text on the y-axis. */
  void setYLabel(const std::string &y_label);

  /** @brief Set the text on the z-axis. */
  void setZLabel(const std::string &z_label);

  /** @brief Set the title of the plot. */
  void setTitle(const std::string &title);

  /** @brief Get the x-label. */
  const juce::Label &getXLabel() const noexcept;

  /** @brief Get the y-label. */
  const juce::Label &getYLabel() const noexcept;

  /** @brief Get the z-label. */
  const juce::Label &getZLabel() const noexcept;

  /** @brief Get the title-label. */
  const juce::Label &getTitleLabel() const noexcept;

  /**
   * @brief Set the camera view, like MATLAB's view(az, el)
   * @param azimuth_degrees horizontal rotation about the z-axis
   * @param elevation_degrees angle above the xy-plane
   */
  void setView(const float azimuth_degrees, const float elevation_degrees);

  /** @brief Get the camera azimuth in degrees. */
  float getViewAzimuth() const noexcept;

  /** @brief Get the camera elevation in degrees. */
  float getViewElevation() const noexcept;

  //==============================================================================
  /** @internal */
  void resized() override;
  /** @internal */
  void paint(juce::Graphics &g) override;
  /** @internal */
  void parentHierarchyChanged() override;
  /** @internal */
  void lookAndFeelChanged() override;

private:
  /** @internal */
  void resizeChildrens();
  /** @internal */
  void resetLookAndFeelChildrens(juce::LookAndFeel *lookandfeel = nullptr);
  /** @internal */
  void updateChildrenParameters();
  /** @internal */
  juce::Rectangle<int> getGraphBounds3D() noexcept;
  /** @internal */
  void updateLabelsIntern();
  /** @internal */
  PlotLookAndFeelBase *getPlotLookAndFeelBase();

  /** Axes and camera view. */
  Axis_f m_x_axis, m_y_axis, m_z_axis;
  float m_azimuth_degrees, m_elevation_degrees;
  juce::Rectangle<int> m_graph_bounds;

  /** Child components */
  std::vector<std::unique_ptr<GraphLine3D>> m_graph_lines;
  std::unique_ptr<Axes3DBox> m_axes_box;
  juce::Label m_x_label, m_y_label, m_z_label, m_title_label;

  /** Look and feel */
  std::unique_ptr<juce::LookAndFeel> m_lookandfeel_default;

  /** Other variables */
  bool m_x_autoscale = true;
  bool m_y_autoscale = true;
  bool m_z_autoscale = true;
};

}  // namespace cmp
