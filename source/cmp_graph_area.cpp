/**
 * Copyright (c) 2022 Frans Rosencrantz
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "cmp_graph_area.h"

#include "cmp_plot.h"

namespace cmp {
void GraphArea::setStartPosition(const juce::Point<int>& start_position) noexcept {
  m_start_pos = start_position;
  m_is_start_pos_set = true;
}

void GraphArea::setEndPosition(const juce::Point<int>& end_position) noexcept {
  m_end_pos = end_position;
}

juce::Point<int> GraphArea::getStartPosition() const noexcept { return m_start_pos; }

juce::Point<int> GraphArea::getEndPosition() const noexcept { return m_end_pos; }

bool GraphArea::isStartPosSet() const noexcept
{
    return m_is_start_pos_set;
}

void GraphArea::reset() noexcept {
  m_start_pos = {0, 0};
  m_end_pos = {0, 0};
  m_is_start_pos_set = false;
}

void GraphArea::resized() {}

void GraphArea::paint(juce::Graphics& g) {
  if (m_lookandfeel && !m_end_pos.isOrigin()) {
    auto* lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);
    lnf->drawSelectionArea(g, m_start_pos, m_end_pos,
                      {{0, 0}, {getWidth(), getHeight()}});
  }
}

void GraphArea::lookAndFeelChanged() {
  if (auto* lnf =
          dynamic_cast<Plot::LookAndFeelMethods*>(&getLookAndFeel())) {
    m_lookandfeel = lnf;
  } else {
    m_lookandfeel = nullptr;
  }
}
}  // namespace cmp
