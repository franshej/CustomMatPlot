/**
 * @file scp_plot.h
 *
 * @brief Componenets for plotting 2-D lines/marker symbols
 *
 * @ingroup SimpleCustomPlot
 *
 * @author Frans Rosencrantz
 * Contact: Frans.Rosencrantz@gmail.com
 *
 */

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "scp_datamodels.h"

namespace scp {

//==============================================================================
/*
 *  \class Plot
 *  \brief A Base Class component to plot 2-D lines/marker symbols.
 *
 *  This class is used to plot 2-D lines/marker symbols. It's also possible to
 *  trace the graph and zoom in/out of specific area. Other featureas: set the
 *  x- and y-limits, ticks and ticklabels. The axis scaling is choosen using the
 *  subclasses below.
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
    linear,     /**< Linear scaling of the graph line. */
    logarithmic /**< Logarithmic scaling of the graph line. */
  };

  /**
     These methods define a interface for the LookAndFeel class of juce.
     The Plot class needs a LookAndFeel, that implements these methods.
     The default implementation can be seen in, \see scp_lookandfeelmethods.h
 */
  class LookAndFeelMethods : public LookAndFeelMethodsBase {
   public:
    virtual ~LookAndFeelMethods(){};

    /** This method draws a frame aroudn the graph area. */
    virtual void drawFrame(juce::Graphics& g,
                           const juce::Rectangle<int> bounds) = 0;

    /** This method draws a single graph line. */
    virtual void drawGraphLine(
        juce::Graphics& g, const std::vector<juce::Point<float>>& graph_points,
        const std::vector<float>& dashed_length, const GraphType graph_type,
        const std::size_t graph_id) = 0;

    /** This method draws the labels on the x and y axis. */
    virtual void drawGridLabels(juce::Graphics& g,
                                const LabelVector& x_axis_labels,
                                const LabelVector& y_axis_labels) = 0;

    /** Returns the 'ColourIdsGraph' for a given id.*/
    virtual ColourIdsGraph getColourFromGraphID(
        const std::size_t graph_id) const = 0;

    /** Get the graph area bounds, where the graphs and grids are to be drawn.*/
    virtual juce::Rectangle<int> getGraphBounds(
        const juce::Rectangle<int>& bounds) const noexcept = 0;

    /** Returns the Font used when drawing the grid labels. */
    virtual juce::Font getGridLabelFont() const noexcept = 0;

    /** Get the bounds of the componenet */
    virtual juce::Rectangle<int> getPlotBounds(
        juce::Rectangle<int>& bounds) const noexcept = 0;

    /** Returns the Font used when drawing the x-, y-axis and title labels. */
    virtual juce::Font getXYTitleFont() const noexcept = 0;

    /** Defines the default colours */
    virtual void setDefaultPlotColours() noexcept = 0;

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

    /** Updates the x-cordinates of the graph points used when drawing a graph
     * line. */
    virtual void updateXGraphPoints(const juce::Rectangle<int>& bounds,
                                    const Scaling scaling, const Lim_f& lim,
                                    const std::vector<float>& x_data,
                                    GraphPoints& graph_points) noexcept = 0;

    /** Updates the y-cordinates of the graph points used when drawing a graph
     * line. */
    virtual void updateYGraphPoints(const juce::Rectangle<int>& bounds,
                                    const Plot::Scaling scaling,
                                    const Lim_f& y_lim,
                                    const std::vector<float>& y_data,
                                    GraphPoints& graph_points) noexcept = 0;

    /** Updates the vertical grid lines with ticks. */
    virtual void updateGridLabelsVertical(
        const juce::Rectangle<int>& bounds,
        const GridLines& vertical_grid_lines,
        const std::vector<float>& custom_x_ticks, StringVector& custom_x_labels,
        LabelVector& x_axis_labels) = 0;

    /** Updates the hotizonal grid lines with ticks. */
    virtual void updateGridLabelsHorizontal(
        const juce::Rectangle<int>& bounds,
        const GridLines& horizontal_grid_lines,
        const std::vector<float>& custom_y_ticks, StringVector& custom_y_labels,
        LabelVector& y_axis_labels) = 0;

    /** Updates the hotizonal grid lines */
    virtual void updateXYTitleLabels(const juce::Rectangle<int>& bounds,
                                     juce::Label& x_label, juce::Label& y_label,
                                     juce::Label& title_label) = 0;

    /*
        virtual void drawBackground(juce::Graphics &g,
                                juce::Rectangle<int> &bounds) = 0;
                                    */
  };

  /** Destructor, making sure to set the lookandfeel in all subcomponenets to
   * nullptr. */
  ~Plot();

  /** @brief Set the X-limits
   *
   *  Set the limits of X-axis.
   *
   *  @param min minimum value
   *  @param max maximum value
   *  @return void.
   */
  void xLim(const float min, const float max);

  /** @brief Set the Y-limits
   *
   *  Set the limits of Y-axis.
   *
   *  @param min minimum value
   *  @param max maximum value
   *  @return void.
   */
  void yLim(const float min, const float max);

  /** @brief Update the Y-data for the graph lines
   *
   *  Update the Y-data. Each vector is the Y-data for a new graph line.
   *  E.g. If 'y_data.size() == 3', three graph lines will be drawn. The
   *  x-values are provided using 'updateXData'. Linear increasing x-values from
   *  0 with the size of y-values are added if no x-values are given.
   *
   *  @param y_data vector of vectors with the y-values.
   *  @return void.
   */
  void updateYData(const std::vector<std::vector<float>>& y_data);

  /** @brief Update the X-data for the graph lines
   *
   *  Update the X-data. Each vector is the X-data for a graph line.
   *  The x_data vector must have the same size as the y_data vector in
   *  'updateYData'.
   *
   *  @param x_data vector of vectors with the x-values.
   *  @return void.
   */
  void updateXData(const std::vector<std::vector<float>>& x_data);

  /** @brief Set the text for label on the X-axis
   *
   *  Set the text for label on the X-axis.
   *
   *  @param x_label text to be displayed on the x-axis
   *  @return void.
   */
  void setXLabel(const std::string& x_label);

  /** @brief Set the text for label on the Y-axis
   *
   *  Set the text for label on the Y-axis.
   *
   *  @param y_label text to be displayed on the y-axis
   *  @return void.
   */
  void setYLabel(const std::string& y_label);

  /** @brief Set the text for title label
   *
   *  Set the text for title label.
   *
   *  @param title text to be displayed as the title
   *  @return void.
   */
  void setTitle(const std::string& title);

  /** @brief Set the text for grid labels on the x-axis
   *
   *  Set custom text for the grid labels and overrides the labels made based
   *  the x-data.
   *
   *  @param x_labels text for the labels displayed as the x-axis
   *  @return void.
   */
  void setXTickLabels(const std::vector<std::string>& x_labels);

  /** @brief Set the text for tick labels on the y-axis
   *
   *  Set custom text for the tick labels and overrides the labels made based
   *  the y-data.
   *
   *  @param y_labels text for the labels displayed as the y-axis
   *  @return void.
   */
  void setYTickLabels(const std::vector<std::string>& y_labels);

  /** @brief Set the ticks values
   *
   *  Use custom ticks to draw the grid lines and tick labels where you wanted.
   *
   *  @param x_ticks x-postions of ticks
   *  @return void.
   */
  void setXTicks(const std::vector<float>& x_ticks);

  /** @brief Set the ticks values
   *
   *  Use custom ticks to draw the grid lines and tick labels where you wanted.
   *
   *  @param y_ticks y-postions of ticks
   *  @return void.
   */
  void setYTicks(const std::vector<float>& y_ticks);

  /** @brief Make a graph dashed
   *
   *  Use custom dash-lengths to draw a dashed line. e.g. dashed_lengths = {2,
   *  2, 4, 6} will draw a line of 2 pixels, skip 2 pixels, draw 3 pixels, skip 6
   *  pixels, and then repeat.
   *
   *  @param dashed_lengths
   *  @param y_ticks y-postions of ticks
   *  @return void.
   */
  void makeGraphDashed(const std::vector<float>& dashed_lengths,
                       unsigned graph_index);
  /** @brief Turn on grids
   *
   *  Turn on grids and tiny grids.
   *
   *  @param grid_on grids is drawn if true
   *  @param tiny_grid_on tiny grids is drawn if true
   *  @return void.
   */
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
};

/**
 *  \class LinearPlot
 *  \brief Component for plotting 2-D graph lines with linear x and y axis.
 */
struct LinearPlot : Plot {
 public:
  LinearPlot();

  std::unique_ptr<scp::GraphLine> getGraphLine() override;

  std::unique_ptr<BaseGrid> getGrid() override;
};

/**
 *  \class SemiPlotX
 *  \brief Component for plotting 2-D graph lines with logarithmic x axis and
 *  linear y axis.
 */
struct SemiPlotX : Plot {
 public:
  SemiPlotX();

  std::unique_ptr<scp::GraphLine> getGraphLine() override;

  std::unique_ptr<BaseGrid> getGrid() override;
};

}  // namespace scp
