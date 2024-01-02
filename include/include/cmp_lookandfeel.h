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

class PlotLookAndFeel : public Plot::LookAndFeelMethods {
 public:
  PlotLookAndFeel();

  void setDefaultPlotColours() noexcept override;

  void overridePlotColours() noexcept override;

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

  std::map<UserInput, UserInputAction> getDefaultUserInputMapAction()
      const noexcept override;

  std::map<UserInput, UserInputAction> overrideUserInputMapAction(
      std::map<UserInput, UserInputAction> default_user_input_map_action)
      const noexcept override;

  UserInputAction getUserInputAction(
      UserInput user_input) const noexcept override;

  void drawGraphLine(juce::Graphics &g,
                     const GraphLineDataView graph_line_data) override;

  void drawGridLabels(juce::Graphics &g, const LabelVector &x_axis_labels,
                      const LabelVector &y_axis_labels) override;

  void drawFrame(juce::Graphics &g, const juce::Rectangle<int> bounds) override;

  void drawGridLine(juce::Graphics &g, const GridLine &grid_line,
                    const GridType grid_type) override;

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

  void drawSelectionArea(
      juce::Graphics &g, juce::Point<int> &start_coordinates,
      const juce::Point<int> &end_coordinates,
      const juce::Rectangle<int> &graph_bounds) noexcept override;

  void updateXGraphPoints(
      const std::vector<std::size_t> &update_only_these_indices,
      const CommonPlotParameterView &common_plot_parameter_view,
      const std::vector<float> &x_data,
      std::vector<std::size_t> &graph_points_indices,
      GraphPoints &graph_points) noexcept override;

  void updateYGraphPoints(
      const std::vector<std::size_t> &update_only_these_indices,
      const CommonPlotParameterView &common_plot_parameter_view,
      const std::vector<float> &y_data,
      const std::vector<std::size_t> &graph_points_indices,
      GraphPoints &graph_points) noexcept override;

  void updateVerticalGridLineTicksAuto(
      const juce::Rectangle<int> &bounds,
      const CommonPlotParameterView &common_plot_parameter_view,
      const GridType grid_type, const std::vector<float> &previous_ticks,
      std::vector<float> &x_ticks) noexcept override;

  void updateHorizontalGridLineTicksAuto(
      const juce::Rectangle<int> &bounds,
      const CommonPlotParameterView &common_plot_parameter_view,
      const GridType grid_type, const std::vector<float> &previous_ticks,
      std::vector<float> &y_ticks) noexcept override;

  juce::Font getGridLabelFont() const noexcept override;

  juce::Font getXYTitleFont() const noexcept override;

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

  bool isXAxisLabelsBelowGraph() const noexcept override;
};  // class PlotLookAndFeel

}  // namespace cmp