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
#include <span>
#include <vector>

#include "cmp_datamodels.h"
#include "cmp_lookandfeel_base.h"

namespace cmp {

class Series3D;
class Axes3DBox;

/** @brief One 3-D series to plot: its data plus optional styling.
 *
 * The fields are ordered x, y, z; all three are required and must have the
 * same size. With C++20 designated initializers a series reads clearly at the
 * call site, e.g.
 * @code
 *   plot3({ {.x = x, .y = y, .z = z} });
 * @endcode
 */
struct Series3DData {
  /** x-values. Must have the same size as y and z. */
  std::vector<float> x;

  /** y-values. Must have the same size as x and z. */
  std::vector<float> y;

  /** z-values. Must have the same size as x and y. */
  std::vector<float> z;

  /** Optional per-series styling. @see SeriesAttribute */
  SeriesAttribute attribute{};
};

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
   * @brief Plot one or more 3-D series.
   * @details Each @ref Series3DData carries its own x, y, z and optional
   *          styling. The x/y/z vectors of a series must have the same size.
   *          The axis limits are auto-scaled to the data unless they have been
   *          set explicitly. Passing an empty list clears the series.
   * @param series the 3-D series to plot @see Series3DData
   */
  void plot3(const std::vector<Series3DData> &series);

  /**
   * @brief Plot a single 3-D series.
   *
   * Convenience overload so a single series need not be wrapped in an extra
   * pair of braces: @code plot3({.x = x, .y = y, .z = z}); @endcode
   *
   * @param series the 3-D series to plot @see Series3DData
   */
  void plot3(const Series3DData &series);

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
  /** @internal Copy the given series' data into the plot's components (one
   * copy each); shared by the plot3(Series3DData) and
   * plot3(std::vector<Series3DData>) overloads. */
  void plot3Series(std::span<const Series3DData> series);
  /** @internal */
  void resizeChildrens();
  /** @internal */
  void resetLookAndFeelChildrens(juce::LookAndFeel *lookandfeel = nullptr);
  /** @internal */
  void updateChildrenParameters();
  /** @internal */
  juce::Rectangle<int> getAxesBounds3D() noexcept;
  /** @internal */
  void updateLabelsIntern();
  /** @internal */
  PlotLookAndFeelBase *getPlotLookAndFeelBase();

  /** Axes and camera view. */
  Axis_f m_x_axis, m_y_axis, m_z_axis;
  float m_azimuth_degrees, m_elevation_degrees;
  juce::Rectangle<int> m_axes_bounds;

  /** Child components */
  std::vector<std::unique_ptr<Series3D>> m_series;
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
