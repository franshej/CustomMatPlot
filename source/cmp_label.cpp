/**
 * Copyright (c) 2022 Frans Rosencrantz
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "cmp_label.h"

#include "cmp_plot.h"

namespace cmp {
PlotLabel::PlotLabel() {
  addAndMakeVisible(m_x_label);
  addAndMakeVisible(m_y_label);
  addAndMakeVisible(m_title_label);
}

void PlotLabel::setXLabel(const std::string &x_label) {
  m_x_label.setText(x_label, juce::NotificationType::dontSendNotification);

  updateLabels();
}

void PlotLabel::setYLabel(const std::string &y_label) {
  m_y_label.setText(y_label, juce::NotificationType::dontSendNotification);

  updateLabels();
}

void PlotLabel::setTitle(const std::string &title) {
  m_title_label.setText(title, juce::NotificationType::dontSendNotification);

  updateLabels();
}

const juce::Label &PlotLabel::getXLabel() const noexcept {
  return m_x_label;
}

const juce::Label &PlotLabel::getYLabel() const noexcept {
  return m_y_label;
}

const juce::Label &PlotLabel::getTitleLabel() const noexcept {
  return m_title_label;
}

const IsLabelsSet PlotLabel::getIsLabelsAreSet() const noexcept {
  return IsLabelsSet{
      m_x_label.getText().isNotEmpty(),
      m_y_label.getText().isNotEmpty(),
      m_title_label.getText().isNotEmpty(),
  };
}

void PlotLabel::resized() { updateLabels(); }

void PlotLabel::updateLabels() {
  if (m_lookandfeel) {
    auto lnf = static_cast<Plot::LookAndFeelMethods *>(m_lookandfeel);
    const auto graph_bounds =
        lnf->getGraphBounds(getBounds(), getParentComponent());
    lnf->updateXYTitleLabels(getBounds(), graph_bounds, m_x_label, m_y_label,
                             m_title_label);
  }
}

void PlotLabel::paint(juce::Graphics &g) {}

void PlotLabel::lookAndFeelChanged() {
  if (auto *lnf = dynamic_cast<Plot::LookAndFeelMethods *>(&getLookAndFeel())) {
    m_lookandfeel = lnf;
    updateLabels();
  } else {
    m_lookandfeel = nullptr;
  }
}
}  // namespace cmp