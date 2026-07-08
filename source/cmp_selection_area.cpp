/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "cmp_selection_area.h"

#include <algorithm>

#include "cmp_plot.h"
#include "cmp_utils.h"

namespace cmp {
void SelectionArea::setStartPosition(
    const juce::Point<int>& start_position) noexcept {
  m_start_pos = start_position;
  m_is_start_pos_set = true;
}

void SelectionArea::setEndPosition(const juce::Point<int>& end_position) noexcept {
  m_end_pos = end_position;
}

juce::Point<int> SelectionArea::getStartPosition() const noexcept {
  return m_start_pos;
}

juce::Point<int> SelectionArea::getEndPosition() const noexcept {
  return m_end_pos;
}

bool SelectionArea::isStartPosSet() const noexcept { return m_is_start_pos_set; }

void SelectionArea::reset() noexcept {
  m_start_pos = {0, 0};
  m_end_pos = {0, 0};
  m_is_start_pos_set = false;
}

void SelectionArea::observableValueUpdated(ObserverId id, const Lim<float> &new_value)
{
  if (id == ObserverId::XLim) {
    m_x_lim = new_value;
  } else if (id == ObserverId::YLim) {
    m_y_lim = new_value;
  }
}

void SelectionArea::observableValueUpdated(ObserverId id, const Scaling &new_value) {
  if (id == ObserverId::XScaling) {
    m_x_scaling = new_value;
  } else if (id == ObserverId::YScaling) {
    m_y_scaling = new_value;
  }
}

void SelectionArea::resized() {}

void SelectionArea::paint(juce::Graphics& g) {
  if (m_lookandfeel && !m_end_pos.isOrigin()) {
    auto* lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);
    lnf->drawSelectionArea(g, m_start_pos, m_end_pos,
                           {{0, 0}, {getWidth(), getHeight()}});
  }
}

void SelectionArea::lookAndFeelChanged() {
  if (auto* lnf = dynamic_cast<Plot::LookAndFeelMethods*>(&getLookAndFeel())) {
    m_lookandfeel = lnf;
  } else {
    m_lookandfeel = nullptr;
  }
}

template <typename ValueType>
juce::Rectangle<ValueType> SelectionArea::getDataBound() const noexcept {
  const auto local_axes_bounds = this->getLocalBounds().toFloat();

  const auto start_pos = this->getStartPosition();
  const auto end_pos = this->getEndPosition();

  const auto x_start = getXDataFromXPixelCoordinate(
      float(start_pos.getX()), local_axes_bounds, m_x_lim, m_x_scaling);
  const auto x_end = getXDataFromXPixelCoordinate(
      float(end_pos.getX()), local_axes_bounds, m_x_lim,
      m_x_scaling);

  const auto y_start = getYDataFromYPixelCoordinate(
      float(start_pos.getY()), local_axes_bounds, m_y_lim,
      m_y_scaling);
  const auto y_end = getYDataFromYPixelCoordinate(
      float(end_pos.getY()), local_axes_bounds, m_y_lim,
      m_y_scaling);

  const auto x_min = std::min(x_start, x_end);
  const auto x_width = std::max(x_start, x_end) - x_min;
  const auto y_min = std::min(y_start, y_end);
  const auto y_height = std::max(y_start, y_end) - y_min;

  return juce::Rectangle<ValueType>{x_min, y_min, x_width, y_height};
};

template juce::Rectangle<float> SelectionArea::getDataBound() const noexcept;

template <typename ValueType>
juce::Rectangle<ValueType> SelectionArea::getSelectedAreaBound() const noexcept {
  const auto start_pos = this->getStartPosition();
  const auto end_pos = this->getEndPosition();

  const auto x_min = std::min(start_pos.getX(), end_pos.getX());
  const auto x_width = std::max(start_pos.getX(), end_pos.getX()) - x_min;
  const auto y_min = std::min(start_pos.getY(), end_pos.getY());
  const auto y_height = std::max(start_pos.getY(), end_pos.getY()) - y_min;

  return juce::Rectangle<ValueType>{x_min, y_min, x_width, y_height};
};

template juce::Rectangle<int> SelectionArea::getSelectedAreaBound() const noexcept;

}  // namespace cmp
