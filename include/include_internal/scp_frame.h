/**
 * @file scp_frame.h
 *
 * @brief Componenet for creating a frame
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
 * \brief A class for creating a frame
 *
 * The idea is to use this componenet to draw the frame around the graph area
 */
class Frame : public juce::Component {
 public:
  Frame() = default;
  ~Frame() = default;

  void resized() override;
  void paint(juce::Graphics& g) override;
  void lookAndFeelChanged() override;

 private:
  LookAndFeelMethodsBase* m_lookandfeel;
};
}  // namespace scp
