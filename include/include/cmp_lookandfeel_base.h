/**
 * @file cmp_lookandfeel_base.h
 * @brief The dimension-agnostic part of the plot LookAndFeel interface
 * @ingroup CustomMatPlot
 * @details This interface contains the LookAndFeel methods that do not depend
 *          on the dimensionality of the plot: backgrounds, frame, fonts,
 *          margins, colours, legend, labels, trace points and user input.
 *          The 2D-specific methods live in Plot::LookAndFeelMethods.
 * @author Frans Rosencrantz
 * @contact Frans.Rosencrantz@gmail.com
 */

#pragma once

#include "cmp_datamodels.h"

namespace cmp {

/**
 *   These methods define the dimension-agnostic part of the LookAndFeel
 *   interface used by the plot components. The 2D-specific remainder is
 *   defined in Plot::LookAndFeelMethods. The default implementation can be
 *   seen in \see cmp_lookandfeel.h
 */
class PlotLookAndFeelBase : public juce::LookAndFeel_V4 {
 public:
  virtual ~PlotLookAndFeelBase() = default;

  /** Draw background. */
  virtual void drawBackground(juce::Graphics &g,
                              const juce::Rectangle<int> &bound) = 0;

  /** This method draws a frame around the axes area. */
  virtual void drawFrame(juce::Graphics &g,
                         const juce::Rectangle<int> bounds) = 0;

  /** This method draws a single series. */
  virtual void drawSeries(juce::Graphics &g, const SeriesDataView series_data,
                          const juce::Rectangle<int> &series_bounds) = 0;

  /** This method draws the legend. */
  virtual void drawLegend(juce::Graphics &g,
                          std::vector<LegendLabel> legend_info,
                          const juce::Rectangle<int> &bound) = 0;

  /** This method draws the legend background. */
  virtual void drawLegendBackground(
      juce::Graphics &g, const juce::Rectangle<int> &legend_bound) = 0;

  /** Draw a single trace point. */
  virtual void drawTraceLabel(juce::Graphics &g, const cmp::Label &x_label,
                              const cmp::Label &y_label,
                              const juce::Rectangle<int> bound) = 0;

  /** This method draws the trace label background. */
  virtual void drawTraceLabelBackground(
      juce::Graphics &g, const juce::Rectangle<int> &trace_label_bound) = 0;

  /** Draw trace point. */
  virtual void drawTracePoint(juce::Graphics &g,
                              const juce::Rectangle<int> &bounds) = 0;

  /** This method draws the selection area (e.g. zoom area). */
  virtual void drawSelectionArea(
      juce::Graphics &g, juce::Point<int> &start_coordinates,
      const juce::Point<int> &end_coordinates,
      const juce::Rectangle<int> &axes_bounds) noexcept = 0;

  /** A method to find and get the colour from an id. */
  virtual CONSTEXPR20 juce::Colour findAndGetColourFromId(
      const int colour_id) const noexcept = 0;

  /** Returns the Font used for the Trace and Zoom buttons. */
  virtual CONSTEXPR20 juce::Font getButtonFont() const noexcept = 0;

  /** Returns the 'ColourIdsSeries' for a given index.*/
  virtual CONSTEXPR20 int getColourFromSeriesID(
      const std::size_t series_index) const = 0;

  /** Get the axes bounds, where the series and grids are to be drawn. A plot
   * component can be given to base the axes bounds on the grid anf axis
   * labels. */
  virtual CONSTEXPR20 juce::Rectangle<int> getAxesBounds(
      const juce::Rectangle<int> bounds,
      const juce::Component *plot_comp = nullptr) const noexcept = 0;

  /** Returns the Font used when drawing the grid labels. */
  virtual CONSTEXPR20 juce::Font getGridLabelFont() const noexcept = 0;

  /** Get Maximum allowed characters for grid labels. */
  virtual CONSTEXPR20 std::size_t getMaximumAllowedCharacterGridLabel()
      const noexcept = 0;

  /** Get the legend position */
  virtual CONSTEXPR20 juce::Point<int> getLegendPosition(
      const juce::Rectangle<int> &axes_bounds,
      const juce::Rectangle<int> &legend_bounds) const noexcept = 0;

  /** Get the legend bounds */
  virtual CONSTEXPR20 juce::Rectangle<int> getLegendBounds(
      [[maybe_unused]] const juce::Rectangle<int> &bounds,
      const std::vector<std::string> &label_texts) const noexcept = 0;

  /** Returns the Font used when drawing legends. */
  virtual juce::Font getLegendFont() const noexcept = 0;

  /** Get margin used for labels and axes bounds. */
  virtual CONSTEXPR20 std::size_t getMargin() const noexcept = 0;

  /** Get a smaller margin. */
  virtual CONSTEXPR20 std::size_t getMarginSmall() const noexcept = 0;

  /** Get pixel length of marker symbol. */
  virtual CONSTEXPR20 std::size_t getMarkerLength() const noexcept = 0;

  /** Get the pixel distance a grid/tick label is pushed outside the series
   * bound it sits beside. Used by the 3D axes box to space its tick labels
   * away from the data cube. */
  virtual CONSTEXPR20 int getGridLabelDistanceFromAxesBound()
      const noexcept = 0;

  /** Get the pixel distance an axis title (the x/y/z label) is pushed outside
   * the axes bound, given the title's pixel height. Used by the 3D plot to
   * place the axis titles just past the tick labels. */
  virtual CONSTEXPR20 int getAxisLabelDistanceFromAxesBound(
      const int label_height) const noexcept = 0;

  /** Get the bounds of the componenet (Local bounds). */
  virtual CONSTEXPR20 juce::Rectangle<int> getPlotBounds(
      juce::Rectangle<int> bounds) const noexcept = 0;

  /** Get the Font used when drawing trace labels. */
  virtual CONSTEXPR20 juce::Font getTraceFont() const noexcept = 0;

  /** Get the local bounds used when drawing the trace label (the bounds
   * around the x & y labels).*/
  virtual CONSTEXPR20 juce::Rectangle<int> getTraceLabelLocalBounds(
      const juce::Rectangle<int> &x_label_bounds,
      const juce::Rectangle<int> &y_label_bounds) const noexcept = 0;

  /** Get the local bounds used when drawing the trace point.*/
  virtual CONSTEXPR20 juce::Rectangle<int> getTracePointLocalBounds()
      const noexcept = 0;

  /** Get x and Y trace label bounds */
  virtual std::pair<juce::Rectangle<int>, juce::Rectangle<int>>
  getTraceXYLabelBounds(const std::string_view x_text,
                        const std::string_view y_text) const = 0;

  /** Get the bounds for the trace and zoom button. */
  virtual CONSTEXPR20 std::pair<juce::Rectangle<int>, juce::Rectangle<int>>
  getTraceAndZoomButtonBounds(
      juce::Rectangle<int> axes_bounds) const noexcept = 0;

  /** Returns the Font used when drawing the x-, y-axis and title labels.
   */
  virtual CONSTEXPR20 juce::Font getXYTitleFont() const noexcept = 0;

  /** Get defualt user input map action. */
  virtual std::map<UserInput, UserInputAction> getDefaultUserInputMapAction()
      const noexcept = 0;

  /** Override the default user input map action. */
  virtual std::map<UserInput, UserInputAction> overrideUserInputMapAction(
      std::map<UserInput, UserInputAction> default_user_input_map_action)
      const noexcept = 0;

  /** Get the user input action for a given user input. */
  virtual UserInputAction getUserInputAction(
      UserInput user_input) const noexcept = 0;

  /** Defines the default colours */
  virtual void setDefaultPlotColours() noexcept = 0;

  /** Override the default colours here or use the setCoulour function on the
   * lnf object. */
  virtual void overridePlotColours() noexcept = 0;

  /** Update the title, x and y axis labels. */
  virtual void updateXYTitleLabels(const juce::Rectangle<int> &bounds,
                                   const juce::Rectangle<int> &axes_bounds,
                                   juce::Label &x_label, juce::Label &y_label,
                                   juce::Label &title_label) = 0;
};

}  // namespace cmp
