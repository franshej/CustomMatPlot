#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class PlotLabel : public juce::Component {

public:
  PlotLabel(juce::Font font = juce::Font(20.0f, juce::Font::plain)) {
    m_xlabel.setFont(font);
    m_ylabel.setFont(font);
    m_title.setFont(font);
    m_xlabel.setJustificationType(juce::Justification::centred);
    m_ylabel.setJustificationType(juce::Justification::centred);
    m_title.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(m_xlabel);
    addAndMakeVisible(m_ylabel);
    addAndMakeVisible(m_title);
  };

  void setXLabel(const std::string &x_label);
  void setYLabel(const std::string &y_label);
  void setTitle(const std::string &title);

  void resized() override;
  void paint(juce::Graphics &g) override;
  void setGraphArea(const juce::Rectangle<int> &graph_area) {
    m_graph_area = graph_area;
	resized();
  };

private:
  juce::Label m_xlabel, m_ylabel, m_title;
  juce::Rectangle<int> m_graph_area;
};