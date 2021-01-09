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
  void updateYValues(const std::vector<float> *y_values);
  void updateXValues(const std::vector<float> *x_values);
  void createDashedPath(const std::vector<float> &dashed_lengths);

  void resized() override;
  void paint(juce::Graphics &g) override;

private:
  virtual void calculateYData() = 0;
  virtual void calculateXData() = 0;
  std::vector<float> m_dashed_lengths;

protected:
  float m_x_min, m_x_max, m_y_min, m_y_max;
  std::atomic<bool> nextYBlockReady = false, nextXBlockReady = false;
  std::pair<float, float> x_plot_limits, y_plot_limits;

  std::vector<std::vector<float>> m_x_values;

  const std::vector<float> *m_y_data;
  const std::vector<float> *m_x_data;
  std::vector<juce::Point<float>> m_graph_points;

  const juce::Colour m_graph_colour;
};

struct LinearGraphLine : public GraphLine {
public:
  LinearGraphLine(juce::Colour graph_colour) : GraphLine(graph_colour){};
  LinearGraphLine() : GraphLine(){};

private:
  void calculateYData() override;
  void calculateXData() override;
};

struct LogXGraphLine : public GraphLine {
public:
  LogXGraphLine(juce::Colour graph_colour) : GraphLine(graph_colour){};
  LogXGraphLine() : GraphLine(){};

private:
  void calculateYData() override;
  void calculateXData() override;
};
