#include "spl_label.h"

namespace scp {
PlotLabel::PlotLabel() : m_params(createDefaultGraphicParams()) {
  m_xlabel.setFont(m_params.font);
  m_ylabel.setFont(m_params.font);
  m_title.setFont(m_params.font);
  m_xlabel.setJustificationType(juce::Justification::centred);
  m_ylabel.setJustificationType(juce::Justification::centred);
  m_title.setJustificationType(juce::Justification::centred);

  addAndMakeVisible(m_xlabel);
  addAndMakeVisible(m_ylabel);
  addAndMakeVisible(m_title);
}

void PlotLabel::setXLabel(const std::string &x_label) {
  m_xlabel.setText(x_label, juce::NotificationType::dontSendNotification);
}

void PlotLabel::setYLabel(const std::string &y_label) {
  m_ylabel.setText(y_label, juce::NotificationType::dontSendNotification);
}

void PlotLabel::setTitle(const std::string &title) {
  m_title.setText(title, juce::NotificationType::dontSendNotification);
}

LabelGraphicParams PlotLabel::createDefaultGraphicParams() {
  LabelGraphicParams params;

  params.font = juce::Font(20.0f, juce::Font::plain);
  params.x_margin = 50;
  params.y_margin = 25;
  params.title_margin = 25;

  return params;
}

void PlotLabel::setGraphicParams(const LabelGraphicParams &params) {
  if (params.font) {
    m_params.font = params.font;
  }
  if (params.x_margin) {
    m_params.x_margin = params.x_margin;
  }
  if (params.y_margin) {
    m_params.y_margin = params.y_margin;
  }
  if (params.title_margin) {
    m_params.title_margin = params.title_margin;
  }
};

void PlotLabel::resized() {
  const auto font = juce::Font(m_params.font);

  const auto y_label_width = font.getStringWidth(m_ylabel.getText());
  const auto x_label_width = font.getStringWidth(m_ylabel.getText());
  const auto title_width = font.getStringWidth(m_ylabel.getText());
  const auto font_height = int(font.getHeightInPoints());

  const juce::Rectangle<int> y_area = {
      m_graph_area.getX() / 2 - m_params.y_margin,
      m_graph_area.getY() + m_graph_area.getHeight() / 2 + m_params.y_margin,
      y_label_width, font_height};

  m_ylabel.setBounds(y_area);

  m_ylabel.setTransform(juce::AffineTransform::rotation(
      -juce::MathConstants<float>::halfPi, y_area.getX(), y_area.getY()));

  const auto y_mid_point_bottom =
      (getHeight() - (m_graph_area.getY() + m_graph_area.getHeight())) / 2;

  m_xlabel.setBounds(
      m_graph_area.getX() + m_graph_area.getWidth() / 2 - m_params.x_margin,
      m_graph_area.getY() + m_graph_area.getHeight() + y_mid_point_bottom,
      x_label_width, font_height);

  m_title.setBounds(
      m_graph_area.getX() + m_graph_area.getWidth() / 2 - m_params.title_margin,
      m_graph_area.getY() / 2 - m_params.title_margin / 2, title_width,
      font_height);
}

void PlotLabel::setGraphArea(const juce::Rectangle<int> &grid_area) {
  m_graph_area = grid_area;
  resized();
};

void PlotLabel::paint(juce::Graphics &g) {}
}  // namespace spc