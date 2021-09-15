#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "scp_datamodels.h"

namespace scp {
//==============================================================================
/*
   \class Plot
   \brief A Base Class component to plot 2-D lines/marker symbols
   Y versus the corresponding values in X.

   This class is used to plot 2-D lines/marker symbols. It's also possible to
   trace the graph and zoom in/out of specific area. Other featureas: set the x-
   and y-limits, ticks and labels. The axis scaling is choosen using the
   subclasses below.
*/

struct Plot : juce::Component {
 public:
  enum ColourIds {
    background_colour,   /**< Colour of the background. */
    grid_colour,         /**< Colour of the grids. */
    x_grid_label_colour, /**< Colour of the label for each x-grid line. */
    y_grid_label_colour, /**< Colour of the label for each y-grid line. */
    frame_colour,        /**< Colour of the frame around the graph area. */
    x_label_colour,      /**< Colour of the text on the x-axis. */
    y_label_colour,      /**< Colour of the label on the y-axis. */
    title_label_colour   /**< Colour of the title label. */
  };

  enum ColourIdsGraph {
    first_graph_colour = (1u << 16u), /**< Colour of the first graph. */
    second_graph_colour,              /**< Colour of the second graph. */
    third_graph_colour,               /**< Colour of the third graph. */
    fourth_graph_colour,              /**< Colour of the fourth graph. */
    fifth_graph_colour,               /**< Colour of the fifth graph. */
    sixth_graph_colour                /**< Colour of the sixth graph. */
  };

  /**< Used in LookAndFeel to choose scaling of a graph_lines. */
  enum GraphType : uint32_t {
    GraphLine, /**< Simple graph line. */
    GridLine   /**< GridLine used for the grids.*/
  };

  /**< Used in LookAndFeel to define the scaling of a graph line. */
  enum Scaling : uint32_t {
    linear /**< Linear scaling of the graph line. */,
    logarithmic /**< Logarithmic scaling of the graph line. */
  };

  /**
     These methods define a interface for the LookAndFeel class of juce.
     The Plot class needs a LookAndFeel, that implements these methods.
     The default implementation can be seen in, \see LookAndFeelMethods.h
 */
  class LookAndFeelMethods : public LookAndFeelMethodsBase {
   public:
    virtual ~LookAndFeelMethods(){};

    /** This method draws a single graph line. */
    virtual void drawGraphLine(
        juce::Graphics& g, const std::vector<juce::Point<float>>& graph_points,
        const std::vector<float>& dashed_length, const GraphType graph_type,
        const std::size_t graph_id) = 0;

    /** This method draws the labels on the x and y axis. */
    virtual void drawGridLabels(juce::Graphics& g,
                                const LabelVector& x_axis_labels,
                                const LabelVector& y_axis_labels) = 0;

    /** This method draws a frame aroudn the graph area. */
    virtual void drawFrame(juce::Graphics& g,
                           const juce::Rectangle<int> bounds) = 0;

    /** Defines the default colours */
    virtual void setDefaultPlotColours() noexcept = 0;

    /** Get the bounds of the componenet */
    virtual juce::Rectangle<int> getPlotBounds(
        juce::Rectangle<int>& bounds) const noexcept = 0;

    /** Get the graph area bounds, where the graphs and grids are to be drawn.*/
    virtual juce::Rectangle<int> getGraphBounds(
        const juce::Rectangle<int>& bounds) const noexcept = 0;

    /** Returns the graph colour id from ColourIdsGraph for a given id.*/
    virtual ColourIdsGraph getColourFromGraphID(
        const std::size_t graph_id) const = 0;

    /** Updates the x-ticks with auto generated ticks. */
    virtual void updateVerticalGridLineTicksAuto(
        const juce::Rectangle<int>& bounds,
        const Plot::Scaling vertical_scaling, const bool tiny_grids,
        const Lim_f x_lim, std::vector<float>& x_ticks) noexcept = 0;

    /** Updates the y-ticks with auto generated ticks. */
    virtual void updateHorizontalGridLineTicksAuto(
        const juce::Rectangle<int>& bounds,
        const Plot::Scaling hotizontal_scaling, const bool tiny_grids,
        const Lim_f y_lim, std::vector<float>& y_ticks) noexcept = 0;

    virtual void updateXGraphPoints(const juce::Rectangle<int>& bounds,
                                    const Scaling scaling, const Lim_f& lim,
                                    const std::vector<float>& x_data,
                                    GraphPoints& graph_points) noexcept = 0;

    virtual void updateYGraphPoints(const juce::Rectangle<int>& bounds,
                                    const Plot::Scaling scaling,
                                    const Lim_f& y_lim,
                                    const std::vector<float>& y_data,
                                    GraphPoints& graph_points) noexcept = 0;

    virtual juce::Font getGridLabelFont() const noexcept = 0;

    virtual juce::Font getXYTitleFont() const noexcept = 0;

    virtual void updateGridLabelsVertical(
        const juce::Rectangle<int>& bounds,
        const GridLines& vertical_grid_lines,
        const std::vector<float>& custom_x_ticks, StringVector& custom_x_labels,
        LabelVector& x_axis_labels) = 0;

    virtual void updateGridLabelsHorizontal(
        const juce::Rectangle<int>& bounds,
        const GridLines& horizontal_grid_lines,
        const std::vector<float>& custom_y_ticks, StringVector& custom_y_labels,
        LabelVector& y_axis_labels) = 0;

    virtual void updateXYTitleLabels(const juce::Rectangle<int>& bounds,
                                     juce::Label& x_label, juce::Label& y_label,
                                     juce::Label& title_label) = 0;

    /*
        virtual void drawBackground(juce::Graphics &g,
                                juce::Rectangle<int> &bounds) = 0;
                                    */
  };

  ~Plot();

  void xLim(const float min, const float max);
  void yLim(const float min, const float max);

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

  void updateYLim(const float min, const float max);
  void updateXLim(const float min, const float max);

  virtual std::unique_ptr<scp::GraphLine> getGraphLine() = 0;
  virtual std::unique_ptr<BaseGrid> getGrid() = 0;

  bool m_x_autoscale = true, m_y_autoscale = true;

  std::vector<std::unique_ptr<scp::GraphLine>> m_graph_lines;
  std::unique_ptr<BaseGrid> m_grid;
  std::unique_ptr<PlotLabel> m_plot_label;
  std::unique_ptr<Frame> m_frame;

  juce::LookAndFeel* m_lookandfeel;
  std::unique_ptr<PlotLookAndFeel> m_lookandfeel_default;

  std::vector<std::vector<float>> m_y_data, m_x_data;

  float m_horizontal_margin, m_vertical_margin = 0.1f;
};

struct LinearPlot : Plot {
 public:
  LinearPlot();

  std::unique_ptr<scp::GraphLine> getGraphLine() override;

  std::unique_ptr<BaseGrid> getGrid() override;
};

struct SemiPlotX : Plot {
 public:
  SemiPlotX();

  std::unique_ptr<scp::GraphLine> getGraphLine() override;

  std::unique_ptr<BaseGrid> getGrid() override;
};

}  // namespace scp
