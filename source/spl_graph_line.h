#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "spl_utils.h"

//template <class ValueType>
//typename std::vector<juce::Point<ValueType>> GraphPoints;

struct GraphLine : juce::Component {
 public:
  GraphLine(const juce::Colour graph_line_colour =
                juce::Colour(std::rand() % 100 + 100, std::rand() % 100 + 100,
                             std::rand() % 100 + 100))
      : m_graph_colour(graph_line_colour){};
  ~GraphLine() = default;

  void setXLim(const float min, const float max);
  void setYLim(const float min, const float max);

  void setYValues(const std::vector<float> &y_values);
  void setXValues(const std::vector<float> &x_values);

  const std::vector<float> &getYValues();
  const std::vector<float> &getXValues();

  void setDashedPath(const std::vector<float> &dashed_lengths);
  void setGraphColour(const juce::Colour &graph_colour);

  void resized() override;
  void paint(juce::Graphics &g) override;

  void calculateYData();
  void calculateXData();

 private:
  std::vector<float> m_dashed_lengths;

  virtual void calculateYDataIntern() = 0;
  virtual void calculateXDataIntern() = 0;

 protected:
  scp::ParamVal<scp::Lim_f> m_x_lim, m_y_lim;
  scp::ParamVal<juce::Colour> m_graph_colour;

  std::vector<float> m_x_data, m_y_data;
  std::vector<juce::Point<float>> m_graph_points;
};

struct LinearGraphLine : public GraphLine {
 public:
  using GraphLine::GraphLine;

 private:
  void calculateYDataIntern() override;
  void calculateXDataIntern() override;
};

struct LogXGraphLine : public GraphLine {
 public:
  using GraphLine::GraphLine;

 private:
  void calculateYDataIntern() override;
  void calculateXDataIntern() override;
};
