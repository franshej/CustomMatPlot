#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

struct GraphLine : juce::Component {
 public:
  GraphLine(const juce::Colour graph_line_colour =
                juce::Colour(std::rand() % 100 + 100, std::rand() % 100 + 100,
                             std::rand() % 100 + 100))
      : m_graph_colour(graph_line_colour){};
  ~GraphLine() = default;

  void xLim(const float &min, const float &max);
  void yLim(const float &min, const float &max);

  void setYValues(const std::vector<float> &y_values);
  void setXValues(const std::vector<float> &x_values);
  void setDashedPath(const std::vector<float> &dashed_lengths);

  const std::vector<float> &getYValues();
  const std::vector<float> &getXValues();

  void resized() override;
  void paint(juce::Graphics &g) override;

  void calculateYData();
  void calculateXData();

  virtual void calculateYDataIntern() = 0;
  virtual void calculateXDataIntern() = 0;

 private:
  std::vector<float> m_dashed_lengths;

 protected:
  float m_x_min_lim, m_x_max_lim, m_y_min_lim, m_y_max_lim;

  std::pair<float, float> x_plot_limits, y_plot_limits;

  std::vector<float> m_x_data, m_y_data;
  std::vector<juce::Point<float>> m_graph_points;

  const juce::Colour m_graph_colour;
};

struct LinearGraphLine : public GraphLine {
 public:
  LinearGraphLine(juce::Colour graph_colour) : GraphLine(graph_colour){};
  LinearGraphLine() : GraphLine(){};

  void calculateYDataIntern() override;
  void calculateXDataIntern() override;
};

struct LogXGraphLine : public GraphLine {
 public:
  LogXGraphLine(juce::Colour graph_colour) : GraphLine(graph_colour){};
  LogXGraphLine() : GraphLine(){};

  void calculateYDataIntern() override;
  void calculateXDataIntern() override;
};
