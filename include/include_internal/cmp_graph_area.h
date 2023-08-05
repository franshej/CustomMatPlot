/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

/**
 * @file cmp_graph_area.h
 *
 * @brief A class for drawing selection overlay on the graph area.
 *
 * @ingroup CustomMatPlotInternal
 *
 * @author Frans Rosencrantz
 * Contact: Frans.Rosencrantz@gmail.com
 *
 */

#pragma once

#include <cmp_datamodels.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace cmp {
/**
 * \class GraphArea #TODO find a better name for this class. Maybe GraphOverlay,
 * GraphSelectionArea? \brief A component for drawing the graph selection on the
 * graph area.
 *
 * The idea is to use this component to draw the zoom area, selection area or
 * move a trace point.
 */
class GraphArea : public juce::Component {
 public:
  /** @brief Constructor.
   *
   *  Constructor.
   *
   * @param common_plot_parameter_view the common plot parameters.
   *  @return void.
   */
  GraphArea(const CommonPlotParameterView& common_plot_parameter_view) noexcept
      : m_common_plot_params(&common_plot_parameter_view){};

  /** @brief Get the start postion.
   *
   *  Get the start postion.
   *
   *  @return a point of the start postion.
   */
  juce::Point<int> getStartPosition() const noexcept;

  /** @brief Get the end zoom area postion.
   *
   *  Get the end screen postion of zoom area.
   *
   *  @return a point of the end postion.
   */
  juce::Point<int> getEndPosition() const noexcept;

  /** @brief Check if start position is set.
   *
   *  Return true if setStartPosition has been called. Returns false if reset
   *  has been called.
   *
   *  @return bool if start position has been set.
   */
  bool isStartPosSet() const noexcept;

  /** @brief Reset the start and end postions.
   *
   *  Set the start and end postion to 0 -> {x = 0,y = 0}.
   *
   *  @return void.
   */
  void reset() noexcept;

  /** @brief Set the start postion.
   *
   *  Set the start screen postion of zoom area.
   *
   *  @param start_position the start position of the zoom area.
   *  @return void.
   */
  void setStartPosition(const juce::Point<int>& start_position) noexcept;

  /** @brief Set the end zoom area postion.
   *
   *  Set the end screen postion of zoom area.
   *
   *  @param end_position the end position of the zoom area.
   *  @return void.
   */
  void setEndPosition(const juce::Point<int>& end_position) noexcept;

  /** @brief Get data bound of selected area.
   *
   *  The data bound can be compare with the graph values.
   *
   *  @return rectangle of the selected area.
   */
  template <typename ValueType>
  juce::Rectangle<ValueType> getDataBound() const noexcept;

  /** @brief Get the selected area bound.
   *
   *  The selected area bound is the rectangle of the selected area.
   *
   *  @return rectangle of the selected area.
   */
  template <typename ValueType>
  juce::Rectangle<ValueType> getSelectedAreaBound() const noexcept;

  /** @internal */
  void resized() override;
  /** @internal */
  void paint(juce::Graphics& g) override;
  /** @internal */
  void lookAndFeelChanged() override;

 private:
  juce::LookAndFeel* m_lookandfeel = nullptr;

  juce::Point<int> m_start_pos, m_end_pos;
  bool m_is_start_pos_set{false};

  const CommonPlotParameterView* m_common_plot_params;
};
}  // namespace cmp
