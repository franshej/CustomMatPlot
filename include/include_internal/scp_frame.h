#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

#include "scp_datamodels.h"

namespace scp {
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

