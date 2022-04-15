/**
 * Copyright (c) 2022 Frans Rosencrantz
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "cmp_frame.h"

#include "cmp_plot.h"

namespace cmp {
void Frame::paint(juce::Graphics& g) {
  if (m_lookandfeel) {
    auto* lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);
    lnf->drawFrame(g, getBounds());
  }
}

void Frame::lookAndFeelChanged() {
  if (auto* lnf = dynamic_cast<Plot::LookAndFeelMethods*>(&getLookAndFeel())) {
    m_lookandfeel = lnf;
  } else {
    m_lookandfeel = nullptr;
  }
}

void Frame::resized() {}

}  // namespace cmp