#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "spl_graph_line.h"
#include "spl_grid.h"
#include "spl_label.h"

namespace scp {
class PlotLookAndFeel;

struct Plot : juce::Component {
 public:
  enum ColourIds : std::uint32_t {
    graph_colour = 0xffff0000 /**< Default is showing all channels in the
                                 LevelMeterSource without a border */
  };

  enum GraphType {
    Default, /**< Default is  */
    GridLine /**< Default is showing all channels in the LevelMeterSource
                without a border */
  };

  class LookAndFeelMethods {
   public:
    virtual ~LookAndFeelMethods(){};

    virtual void setDefaultPlotColours() noexcept = 0;

    virtual juce::Rectangle<int> getBounds(
        juce::Rectangle<int>& bounds) const = 0;

    virtual juce::Rectangle<int> getGraphAreaBounds(
        juce::Rectangle<int>& bounds) const = 0;

    /*
        virtual void drawBackground(juce::Graphics &g,
                                juce::Rectangle<int> &bounds) = 0;

       virtual void drawGraph(juce::Graphics &g, juce::Rectangle<int>
       &graph_bound, std::vector<float> &dashed_length) = 0;

    */
  };

  ~Plot() = default;

  void xLim(const float& min, const float& max);
  void yLim(const float& min, const float& max);

  void updateYData(const std::vector<std::vector<float>>& y_data);
  void updateXData(const std::vector<std::vector<float>>& x_data);

  void setXLabel(const std::string& x_label);
  void setYLabel(const std::string& y_label);
  void setTitle(const std::string& title);

  void setXLabels(const std::vector<std::string>& x_labels);
  void setYLabels(const std::vector<std::string>& y_labels);

  void setXTicks(const std::vector<float>& x_ticks);
  void setYTicks(const std::vector<float>& y_ticks);

  void makeGraphDashed(const std::vector<float>& dashed_lengths,
                       unsigned graph_index);
  void gridON(const bool grid_on, const bool tiny_grid_on);

  void resized() override;
  void paint(juce::Graphics& g) override;

  void parentHierarchyChanged() override;
  void lookAndFeelChanged() override;

 protected:
  void initialize();

 private:
  void setAutoXScale();
  void setAutoYScale();

  void updateYLim(const float& min, const float& max);
  void updateXLim(const float& min, const float& max);

  virtual std::unique_ptr<scp::GraphLine> getGraphLine() = 0;
  virtual std::unique_ptr<BaseGrid> getGrid() = 0;

  bool m_x_autoscale = true, m_y_autoscale = true;

  std::vector<std::unique_ptr<scp::GraphLine>> m_graph_lines;
  std::unique_ptr<BaseGrid> m_grid;

  LookAndFeelMethods* m_lookandfeel;
  std::unique_ptr<PlotLookAndFeel> m_lookandfeel_default;
  PlotLabel m_plot_label;

  juce::Rectangle<int> m_graph_area, m_plot_area;
  std::vector<std::vector<float>> m_y_data, m_x_data;

  float m_horizontal_margin, m_vertical_margin = 0.1f;
};

struct LinearPlot : Plot {
 public:
  LinearPlot();

  std::unique_ptr<scp::GraphLine> getGraphLine() override {
    return std::move(std::make_unique<scp::LinearGraphLine>());
  }

  std::unique_ptr<BaseGrid> getGrid() override {
    return std::move(std::make_unique<Grid>());
  }
};

struct SemiPlotX : Plot {
 public:
  SemiPlotX();

  std::unique_ptr<scp::GraphLine> getGraphLine() override {
    return std::move(std::make_unique<scp::LogXGraphLine>());
  }

  std::unique_ptr<BaseGrid> getGrid() override {
    return std::move(std::make_unique<SemiLogXGrid>());
  }
};

}  // namespace scp
