/**
 * @file cmp_lookandfeel.h
 *
 * @brief LookAndFeel class.
 *
 * @ingroup Custom Mat Plot
 *
 * @author Frans Rosencrantz
 * Contact: Frans.Rosencrantz@gmail.com
 */

#pragma once

#include "cmp_plot.h"

namespace cmp {

/*
 *  \class PlotLookAndFeel
 *  \brief LookAndFeel class for user custom lookanfeel.
 *
 *  Inherence from this class if a custom lookandfeel class is wanted.
 *  Override the functions wanted to be changed.
 */
class PlotLookAndFeel;

/*
 *  \class PlotLookAndFeelDefault
 *  \brief A template LookAndFeel class.
 *
 *  Do not directly inherence from this class, use 'PlotLookAndFeel' instead.
 */
template <Scaling x_scaling_t, Scaling y_scaling_t>
class PlotLookAndFeelDefault : public Plot::LookAndFeelMethods {
  static constexpr auto m_x_scaling = x_scaling_t;
  static constexpr auto m_y_scaling = y_scaling_t;

 public:
  PlotLookAndFeelDefault();

  void setDefaultPlotColours() noexcept override;

  void drawBackground(juce::Graphics &g,
                      const juce::Rectangle<int> &bound) override;

  juce::Colour findAndGetColourFromId(
      const int colour_id) const noexcept override;

  juce::Rectangle<int> getPlotBounds(
      juce::Rectangle<int> bounds) const noexcept override;

  std::pair<juce::Rectangle<int>, juce::Rectangle<int>>
  getTraceAndZoomButtonBounds(
      juce::Rectangle<int> graph_bounds) const noexcept override;

  juce::Rectangle<int> getGraphBounds(
      const juce::Rectangle<int> bounds,
      const juce::Component *const plot_comp = nullptr) const noexcept override;

  std::size_t getMaximumAllowedCharacterGridLabel() const noexcept override;

  juce::Point<int> getLegendPosition(
      const juce::Rectangle<int> &graph_bounds,
      const juce::Rectangle<int> &legend_bounds) const noexcept override;

  juce::Rectangle<int> getLegendBounds(
      [[maybe_unused]] const juce::Rectangle<int> &graph_bounds,
      const std::vector<std::string> &label_texts) const noexcept override;

  juce::Font getLegendFont() const noexcept override;

  juce::Font getButtonFont() const noexcept override;

  int getColourFromGraphID(const std::size_t graph_index) const override;

  std::size_t getMargin() const noexcept override;

  std::size_t getMarginSmall() const noexcept override;

  std::size_t getMarkerLength() const noexcept override;

  std::pair<juce::Rectangle<int>, juce::Rectangle<int>> getTraceXYLabelBounds(
      const std::string_view x_text,
      const std::string_view y_text) const override;

  juce::Rectangle<int> getTraceLabelLocalBounds(
      const juce::Rectangle<int> &x_label_bounds,
      const juce::Rectangle<int> &y_label_bounds) const noexcept override;

  juce::Rectangle<int> getTracePointLocalBounds() const noexcept override;

  juce::Font getTraceFont() const noexcept override;

  juce::Point<int> getTracePointPositionFrom(
      const CommonPlotParameterView common_plot_params,
      const juce::Point<float> graph_values) const noexcept override;

  int getXGridLabelDistanceFromGraphBound() const noexcept override;

  int getYGridLabelDistanceFromGraphBound(
      const int y_grid_label_width) const noexcept override;

  void drawGraphLine(juce::Graphics &g,
                     const GraphLineDataView graph_line_data) override;

  void drawGridLabels(juce::Graphics &g, const LabelVector &x_axis_labels,
                      const LabelVector &y_axis_labels) override;

  void drawFrame(juce::Graphics &g, const juce::Rectangle<int> bounds) override;

  void drawGridLine(juce::Graphics &g, const GridLine &grid_line,
                    const bool grid_on) override;

  void drawLegend(juce::Graphics &g, std::vector<LegendLabel> legend_info,
                  const juce::Rectangle<int> &bounds) override;

  void drawLegendBackground(juce::Graphics &g,
                            const juce::Rectangle<int> &legend_bound) override;

  void drawSpread(juce::Graphics &g, const GraphLine *first_graph,
                  const GraphLine *second_graph,
                  const juce::Colour &spread_colour) override;

  void drawTraceLabel(juce::Graphics &g, const cmp::Label &x_label,
                      const cmp::Label &y_label,
                      const juce::Rectangle<int> bound) override;

  void drawTraceLabelBackground(
      juce::Graphics &g,
      const juce::Rectangle<int> &trace_label_bound) override;

  void drawTracePoint(juce::Graphics &g,
                      const juce::Rectangle<int> &bounds) override;

  void drawZoomArea(juce::Graphics &g, juce::Point<int> &start_coordinates,
                    const juce::Point<int> &end_coordinates,
                    const juce::Rectangle<int> &graph_bounds) noexcept override;

  void updateXGraphPoints(const juce::Rectangle<int> &bounds,
                          const Lim_f &x_lim, const std::vector<float> &x_data,
                          std::vector<std::size_t> &graph_points_indices,
                          GraphPoints &graph_points) noexcept override;

  void updateYGraphPoints(const juce::Rectangle<int> &bounds,
                          const Lim_f &y_lim, const std::vector<float> &y_data,
                          const std::vector<std::size_t> &graph_points_indices,
                          GraphPoints &graph_points) noexcept override;

  void updateVerticalGridLineTicksAuto(
      const juce::Rectangle<int> &bounds, const bool tiny_grids,
      const Lim_f x_lim, std::vector<float> &x_ticks) noexcept override;

  void updateHorizontalGridLineTicksAuto(
      const juce::Rectangle<int> &bounds, const bool tiny_grids,
      const Lim_f y_lim, std::vector<float> &y_ticks) noexcept override;

  juce::Font getGridLabelFont() const noexcept override;

  juce::Font getXYTitleFont() const noexcept override;

  Scaling getXScaling() const noexcept override;

  Scaling getYScaling() const noexcept override;

  void updateGridLabels(const CommonPlotParameterView common_plot_params,
                        const std::vector<GridLine> &grid_lines,
                        StringVector &x_custom_label_ticks,
                        StringVector &y_custom_label_ticks,
                        LabelVector &x_axis_labels_out,
                        LabelVector &y_axis_labels_out) override;

  void updateXYTitleLabels(const juce::Rectangle<int> &bounds,
                           const juce::Rectangle<int> &graph_bounds,
                           juce::Label &x_label, juce::Label &y_label,
                           juce::Label &title_label) override;
};  // class PlotLookAndFeelDefault

class PlotLookAndFeel
    : public PlotLookAndFeelDefault<Scaling::linear, Scaling::linear>,
      public PlotLookAndFeelDefault<Scaling::logarithmic, Scaling::linear>,
      public PlotLookAndFeelDefault<Scaling::linear, Scaling::logarithmic>,
      public PlotLookAndFeelDefault<Scaling::logarithmic,
                                    Scaling::logarithmic> {};

}  // namespace cmp