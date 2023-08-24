/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "cmp_graph_area.h"

#include <algorithm>

#include "cmp_plot.h"
#include "cmp_utils.h"

namespace cmp {
void GraphArea::setStartPosition(
    const juce::Point<int>& start_position) noexcept {
  m_start_pos = start_position;
  m_is_start_pos_set = true;
}

void GraphArea::setEndPosition(const juce::Point<int>& end_position) noexcept {
  m_end_pos = end_position;
}

juce::Point<int> GraphArea::getStartPosition() const noexcept {
  return m_start_pos;
}

juce::Point<int> GraphArea::getEndPosition() const noexcept {
  return m_end_pos;
}

bool GraphArea::isStartPosSet() const noexcept { return m_is_start_pos_set; }

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
  if (auto* lnf = dynamic_cast<Plot::LookAndFeelMethods*>(&getLookAndFeel())) {
    m_lookandfeel = lnf;
  } else {
    m_lookandfeel = nullptr;
  }
}

template <typename ValueType>
juce::Rectangle<ValueType> GraphArea::getDataBound() const noexcept {
  const auto local_graph_bounds = this->getLocalBounds().toFloat();

  const auto start_pos = this->getStartPosition();
  const auto end_pos = this->getEndPosition();

  const auto x_start = getXFromXCoordinate(
      float(start_pos.getX()), local_graph_bounds, m_common_plot_params->x_lim,
      m_common_plot_params->x_scaling);
  const auto x_end = getXFromXCoordinate(
      float(end_pos.getX()), local_graph_bounds, m_common_plot_params->x_lim,
      m_common_plot_params->x_scaling);

  const auto y_start = getYFromYCoordinate(
      float(start_pos.getY()), local_graph_bounds, m_common_plot_params->y_lim,
      m_common_plot_params->y_scaling);
  const auto y_end = getYFromYCoordinate(
      float(end_pos.getY()), local_graph_bounds, m_common_plot_params->y_lim,
      m_common_plot_params->y_scaling);

  const auto x_min = std::min(x_start, x_end);
  const auto x_width = std::max(x_start, x_end) - x_min;
  const auto y_min = std::min(y_start, y_end);
  const auto y_height = std::max(y_start, y_end) - y_min;

  return juce::Rectangle<ValueType>{x_min, y_min, x_width, y_height};
};

template juce::Rectangle<float> GraphArea::getDataBound() const noexcept;

template <typename ValueType>
juce::Rectangle<ValueType> GraphArea::getSelectedAreaBound() const noexcept {
  const auto start_pos = this->getStartPosition();
  const auto end_pos = this->getEndPosition();

  const auto x_min = std::min(start_pos.getX(), end_pos.getX());
  const auto x_width = std::max(start_pos.getX(), end_pos.getX()) - x_min;
  const auto y_min = std::min(start_pos.getY(), end_pos.getY());
  const auto y_height = std::max(start_pos.getY(), end_pos.getY()) - y_min;

  return juce::Rectangle<ValueType>{x_min, y_min, x_width, y_height};
};

template juce::Rectangle<int> GraphArea::getSelectedAreaBound() const noexcept;

}  // namespace cmp
