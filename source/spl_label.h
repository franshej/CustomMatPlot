#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "spl_utils.h"

struct LabelGraphicParams {
  scp::ParamVal<juce::Font> font;
  scp::ParamVal<int> x_margin;
  scp::ParamVal<int> y_margin;
  scp::ParamVal<int> title_margin;
};

class PlotLabel : public juce::Component {
 public:
  PlotLabel();

  void setXLabel(const std::string &x_label);
  void setYLabel(const std::string &y_label);
  void setTitle(const std::string &title);
  void setParams(const LabelGraphicParams &params);

  void resized() override;
  void paint(juce::Graphics &g) override;
  void setGraphArea(const juce::Rectangle<int> &graph_area) {
    m_graph_area = graph_area;
    resized();
  };

 private:
  LabelGraphicParams createDefaultGraphicParams();

  LabelGraphicParams m_params;
  juce::Label m_xlabel, m_ylabel, m_title;
  juce::Rectangle<int> m_graph_area;
};