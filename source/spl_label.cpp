#include "spl_label.h"

void PlotLabel::setXLabel(const std::string &x_label) {
  m_xlabel.setText(x_label, juce::NotificationType::dontSendNotification);
}

void PlotLabel::setYLabel(const std::string &y_label) {
  ;
  m_ylabel.setText(y_label, juce::NotificationType::dontSendNotification);
}

void PlotLabel::setTitle(const std::string &title) {
  m_title.setText(title, juce::NotificationType::dontSendNotification);
}

void PlotLabel::resized() {
  const juce::Rectangle<int> y_area = {
      m_graph_area.getX() / 2 - 50,
      m_graph_area.getY() + m_graph_area.getHeight() / 2 + 50, 100, 50};

  m_ylabel.setBounds(y_area);

  m_ylabel.setTransform(juce::AffineTransform::rotation(
      -juce::MathConstants<float>::halfPi, y_area.getX(), y_area.getY()));

  const auto y_mid_point_bottom =
      (getHeight() - (m_graph_area.getY() + m_graph_area.getHeight())) / 2;
  m_xlabel.setBounds(m_graph_area.getX() + m_graph_area.getWidth() / 2 - 50,
                     m_graph_area.getY() + m_graph_area.getHeight() - 25 +
                         y_mid_point_bottom,
                     100, 50);

  m_title.setBounds(m_graph_area.getX() + m_graph_area.getWidth() / 2 - 50,
                    m_graph_area.getY() / 2 - 25, 100, 50);
}

void PlotLabel::paint(juce::Graphics &g){};