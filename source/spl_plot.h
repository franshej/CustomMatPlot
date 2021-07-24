#pragma once

#include "spl_graph_line.h"
#include "spl_grid.h"
#include "spl_label.h"
#include <juce_gui_basics/juce_gui_basics.h>

struct GraphLineData
{
    std::vector<float> x_data;
    std::vector<float> y_data;
    std::unique_ptr<GraphLine> grpah_line;
};

struct Plot : juce::Component {
public:
  Plot() = default;
  ~Plot() = default;

  void xLim(const float &min, const float &max);
  void yLim(const float &min, const float &max);

  void updateYData(const std::vector<std::vector<float>> &y_data);
  void updateXData(const std::vector<std::vector<float>> &x_data);

  void setXLabel(const std::string &x_label);
  void setYLabel(const std::string &y_label);
  void setTitle(const std::string &title);

  void makeGraphDashed(const std::vector<float> &dashed_lengths,
                       unsigned graph_index);
  void gridON(const bool grid_on);

  void resized() override;
  void paint(juce::Graphics &g) override;

protected:
  void setAutoXScale();
  void setAutoYScale();

  void updateYLim(const float &min, const float &max);
  void updateXLim(const float &min, const float &max);

  virtual std::unique_ptr<GraphLine> getGraphLine() = 0;

  bool m_x_autoscale = true, m_y_autoscale = true;

  std::vector<std::unique_ptr<GraphLine>> m_graph_lines;
  std::unique_ptr<BaseGrid> m_grid;
  PlotLabel m_plot_label;

  juce::Rectangle<int> m_graph_area, m_plot_area;
  std::vector<std::vector<float>> m_y_data, m_x_data;

  float m_horizontal_margin, m_vertical_margin = 0.1f;
};

struct LinearPlot : Plot {
public:
  ~LinearPlot() = default;
  LinearPlot(const int x, const int y, const int width, const int height);
  LinearPlot();

  std::unique_ptr<GraphLine> getGraphLine() override { return std::move(std::make_unique<LinearGraphLine>()); }
};

struct SemiPlotX : Plot {
public:
  ~SemiPlotX() = default;
  SemiPlotX();

  std::unique_ptr<GraphLine> getGraphLine() override { return std::move(std::make_unique<LogXGraphLine>()); }
};
