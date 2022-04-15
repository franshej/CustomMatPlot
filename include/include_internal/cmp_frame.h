/**
 * Copyright (c) 2022 Frans Rosencrantz
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

/**
 * @file cmp_frame.h
 *
 * @brief Componenet for creating a frame
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
 * \brief A class for creating a frame
 *
 * The idea is to use this componenet to draw the frame around the graph area
 */
class Frame : public juce::Component {
 public:
  void resized() override;
  void paint(juce::Graphics& g) override;
  void lookAndFeelChanged() override;

 private:
  juce::LookAndFeel* m_lookandfeel;
};
}  // namespace cmp
