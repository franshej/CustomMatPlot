#include "spl_label.h"

#include "spl_plot.h"

namespace scp {
PlotLabel::PlotLabel() {
  addAndMakeVisible(m_x_label);
  addAndMakeVisible(m_y_label);
  addAndMakeVisible(m_title_label);
}

void PlotLabel::setXLabel(const std::string &x_label) {
  m_x_label.setText(x_label, juce::NotificationType::dontSendNotification);
}

void PlotLabel::setYLabel(const std::string &y_label) {
  m_y_label.setText(y_label, juce::NotificationType::dontSendNotification);
}

void PlotLabel::setTitle(const std::string &title) {
  m_title_label.setText(title, juce::NotificationType::dontSendNotification);
}

void PlotLabel::resized() {
  if (m_lookandfeel) {
    auto lnf = static_cast<Plot::LookAndFeelMethods *>(m_lookandfeel);
    lnf->updateXYTitleLabels(getBounds(), m_x_label, m_y_label, m_title_label);
  }
}

void PlotLabel::paint(juce::Graphics &g) {}
void PlotLabel::lookAndFeelChanged() {
  if (auto *lnf = dynamic_cast<Plot::LookAndFeelMethods *>(&getLookAndFeel())) {
    m_lookandfeel = lnf;
  } else {
    m_lookandfeel = nullptr;
  }
}
}  // namespace scp