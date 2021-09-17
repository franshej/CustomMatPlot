#include "scp_frame.h"

#include "spl_plot.h"

namespace scp {
void Frame::paint(juce::Graphics& g) {
  if (m_lookandfeel) {
    auto* lnf = static_cast<PlotBase::LookAndFeelMethods*>(m_lookandfeel);
    lnf->drawFrame(g, getBounds());
  }
}

void Frame::lookAndFeelChanged() {
  if (auto* lnf = dynamic_cast<PlotBase::LookAndFeelMethods*>(&getLookAndFeel())) {
    m_lookandfeel = lnf;
  } else {
    m_lookandfeel = nullptr;
  }
}

void Frame::resized() {}

}  // namespace scp