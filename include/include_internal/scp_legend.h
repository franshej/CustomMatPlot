/**
 * @file scp_legend.h
 *
 * @brief Componenet for legend
 *
 * @ingroup SimpleCustomPlotInternal
 *
 * @author Frans Rosencrantz
 * Contact: Frans.Rosencrantz@gmail.com
 *
 */
#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "scp_datamodels.h"

namespace scp {
/**
 * \class Frame
 * \brief A class for creating legend
 *
 * The idea is to use this componenet to creates a legend with descriptive
 * labels for each plotted data series.
 */
class Legend : public juce::Component {
 public:
  Legend() = default;
  ~Legend() = default;

  /** @brief Set the text for the descriptive labels
   *
   *  Set the text for the descriptive labels for each plotted data series
   *
   *  @param label_texts the text to be displayed for each data series
   *  @return void.
   */
  void setLegend(const std::vector<std::string>& label_texts);

  /** @brief Set data series
   *
   *  Set the data series.
   *
   *  @param data_series poitner to the data series that are plotted.
   *  @return void.
   */
  void setDataSeries(const GraphLines* data_series);

  void resized() override;
  void paint(juce::Graphics& g) override;
  void lookAndFeelChanged() override;

 private:
  LookAndFeelMethodsBase* m_lookandfeel;

  std::vector<std::string> m_label_texts;
  const GraphLines* m_graph_lines{nullptr};
};
}  // namespace scp
