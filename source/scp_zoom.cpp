#include "scp_zoom.h"

#include "spl_plot.h"

namespace scp {
void Zoom::setStartPosition(const juce::Point<int>& start_position) noexcept {
  m_start_pos = start_position;
}

void Zoom::setEndPosition(const juce::Point<int>& end_position) noexcept {
  m_end_pos = end_position;
}

juce::Point<int> Zoom::getStartPosition() const noexcept { return m_start_pos; }
juce::Point<int> Zoom::getEndPosition() const noexcept { return m_end_pos; }

void Zoom::reset() noexcept {
  m_start_pos = {0, 0};
  m_end_pos = {0, 0};
}

void Zoom::resized() {}

void Zoom::paint(juce::Graphics& g) {
  if (m_lookandfeel) {
    auto* lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);
    lnf->drawZoomArea(g, m_start_pos, m_end_pos,
                      {{0, 0}, {getWidth(), getHeight()}});
  }
}

void Zoom::lookAndFeelChanged() {
  if (auto* lnf =
          dynamic_cast<Plot::LookAndFeelMethods*>(&getLookAndFeel())) {
    m_lookandfeel = lnf;
  } else {
    m_lookandfeel = nullptr;
  }
}
}  // namespace scp
