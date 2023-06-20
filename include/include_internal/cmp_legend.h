/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

/**
 * @file cmp_legend.h
 *
 * @brief Componenet for legend
 *
 * @ingroup CustomMatPlotInternal
 *
 * @author Frans Rosencrantz
 * Contact: Frans.Rosencrantz@gmail.com
 *
 */
#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "cmp_datamodels.h"

namespace cmp {
/**
 * \class Frame
 * \brief A class for creating legend
 *
 * The idea is to use this componenet to creates a legend with descriptive
 * labels for each plotted data series.
 */
class Legend : public juce::Component {
 public:
  /** @brief Set the text for the descriptive labels
   *
   *  Set the text for the descriptive labels for each plotted data series
   *
   *  @param label_texts the text to be displayed for each data series
   *  @return void.
   */
  void setLegend(const StringVector& graph_descriptions);

  /** @breif Update legends
   *
   * Update the legends based on info from the graph_lines.
   * The legend descriptions are resized based on the graph_lines size.
   * The colour of the graph_line is displayed before the description text.
   *
   * @param GraphLines* graph_lines in the plot.
   * @return void.
   */
  void updateLegends(const GraphLines& graph_lines);

  /** @internal */
  void resized() override;
  /** @internal */
  void paint(juce::Graphics& g) override;
  /** @internal */
  void lookAndFeelChanged() override;

  /** @breif This lamda is trigged when the number of descriptions is changed.
   *
   * Can be use to resize the legend when the number of descriptions is changed.
   *
   * @param StringVector the descriptions that will be drawn.
   * @return void.
   */
  std::function<void(const StringVector&)> onNumberOfDescriptionsChanged{
      nullptr};

 private:
  juce::LookAndFeel* m_lookandfeel;

  std::vector<LegendLabel> m_legend_labels;
  std::vector<std::string> m_label_texts;

  bool m_label_texts_is_changed{false};
  const GraphLines* m_graph_lines{nullptr};
};
}  // namespace cmp
