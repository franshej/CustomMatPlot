/**
 * Copyright (c) 2022 Frans Rosencrantz
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

/**
 * @file cmp_label.h
 *
 * @brief Componenets for creating x, y and title labels
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
#include "cmp_utils.h"

namespace cmp {
/**
 * \class Plot
 * \brief A class for creating x, y and title labels
 *
 * The idea is to use this componenet to draw the x, y and title labels.
 */
class PlotLabel : public juce::Component {
 public:
  PlotLabel();

  /** @brief Set the x-label text
   *
   *  Set the text that will be display on the x-axis.
   *
   *  @param x_label the text that will be drawn on the x-axis.
   *  @return void.
   */
  void setXLabel(const std::string &x_label);

  /** @brief Set the y-label text
   *
   *  Set the text that will be display on the y-axis.
   *
   *  @param y_label the text that will be drawn on the y-axis.
   *  @return void.
   */
  void setYLabel(const std::string &y_label);

  /** @brief Set the title-label text
   *
   *  Set the text that will be display on the upper x-axis.
   *
   *  @param title_label the text that will be drawn on the upper x-axis
   *  @return void.
   */
  void setTitle(const std::string &title_label);

  /** @brief Get the x-label
   *
   *  Get the x-label reference.
   *
   *  @return const juce::Label reference.
   */
  const juce::Label &getXLabel() const noexcept;

  /** @brief Get the y-label
   *
   *  Get the y-label reference.
   *
   *  @return const juce::Label reference.
   */
  const juce::Label &getYLabel() const noexcept;

  /** @brief Get the title-label
   *
   *  Get the title-label reference.
   *
   *  @return const juce::Label reference.
   */
  const juce::Label &getTitleLabel() const noexcept;

  /** @brief Get info struct about if the labels are set ot not.
   *
   *  @return const IsLabelsSet.
   */
  const IsLabelsSet getIsLabelsAreSet() const noexcept;

  void resized() override;
  void paint(juce::Graphics &g) override;
  void lookAndFeelChanged() override;

 private:
  void updateLabels();

  juce::Label m_x_label, m_y_label, m_title_label;
  juce::LookAndFeel *m_lookandfeel;
};
}  // namespace cmp