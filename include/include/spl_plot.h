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

/*
 *  \class Plot
 *  \brief A Base Class component to plot 2-D lines/marker symbols.
 *
 *  This class can be used to plot 2-D lines/marker symbols. It's also possible
 *  to trace the graph and zoom in/out of specific area. Other featureas: set
 * the x- and y-limits, ticks and ticklabels. Logarithmic scaling is choosen
 *  using the subclasses below.
 */
class Plot : public juce::Component {
 public:
  /** Destructor, making sure to set the lookandfeel in all subcomponenets to
   * nullptr. */
  ~Plot();

  /* Constructor **/
  Plot(const Scaling x_scaling = Scaling::linear,
       const Scaling y_scaling = Scaling::linear);

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

  /** @brief Plot y-data or y-data/x-data.
   *
   *  Plot y-data or y-data/x-data. Each vector in y-data represents a single
   *  graph line. E.g. If 'y_data.size() == 3', three graph lines will be
   *  plotted. If 'x_data' is empty the x-vlues will be set to linearly
   *  increasing from 0 to the size of y-data - 1. If 'graph_colours' is
   *  empty then 'ColourIdsGraph' is used.
   *
   *  @param y_data vector of vectors with the y-values.
   *  @param x_data vector of vectors with the y-values.
   *  @param graph_colours a vector with user defined graph colours.
   *  @return void.
   */
  void plot(const std::vector<std::vector<float>>& y_data,
            const std::vector<std::vector<float>>& x_data = {},
            ColourVector graph_colours = {});

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
   *  2, 4, 6} will draw a line of 2 pixels, skip 2 pixels, draw 3 pixels, skip
   *  6 pixels, and then repeat.
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

  /** @brief Set Legend
   *
   *  Set descriptions for each graph. The label 'label1..N' will be used if
   *  fewers numbers of graph_descriptions are provided than existing numbers of
   *  graphs.
   *
   *  @param graph_descriptions description of the graphs
   *  @return void.
   */
  void setLegend(const std::vector<std::string>& graph_descriptions);

  /** @brief  Sets the look and feel to use for this component.

    The object passed in will not be deleted by the component, so it's the
    caller's responsibility to manage it. It may be used at any time until this
    component has been deleted.

    Calling this method will also invoke the juce::Componenet::setLookAndFeel
    and in turn sendLookAndFeelChange() method.

    @see getLookAndFeel, lookAndFeelChanged
*/
  void setLookAndFeel(PlotLookAndFeel* look_and_feel);

  //==============================================================================

  /** @brief A set of colour IDs to use to change the colour of various aspects
   *  of the Plot.
   *
   *  These constants can be used either via the Component::setColour(), or
   *  LookAndFeel::setColour() methods.
   *
   *  @see Component::setColour, Component::findColour, LookAndFeel::setColour,
   *  LookAndFeel::findColour
   */
  enum ColourIds {
    background_colour,   /**< Colour of the background. */
    grid_colour,         /**< Colour of the grids. */
    x_grid_label_colour, /**< Colour of the label for each x-grid line. */
    y_grid_label_colour, /**< Colour of the label for each y-grid line. */
    frame_colour,        /**< Colour of the frame around the graph area. */
    x_label_colour,      /**< Colour of the text on the x-axis. */
    y_label_colour,      /**< Colour of the label on the y-axis. */
    title_label_colour,  /**< Colour of the title label. */
    legend_label_colour, /**< Colour of the legend label(s). */
    zoom_area_colour     /**< Colour of the dashed zoom rectangle. */
  };

  /** @brief A set of colour IDs to use to change the colour of each plot
   * line.*/
  enum ColourIdsGraph {
    first_graph_colour = (1u << 16u), /**< Colour of the first graph. */
    second_graph_colour,              /**< Colour of the second graph. */
    third_graph_colour,               /**< Colour of the third graph. */
    fourth_graph_colour,              /**< Colour of the fourth graph. */
    fifth_graph_colour,               /**< Colour of the fifth graph. */
    sixth_graph_colour                /**< Colour of the sixth graph. */
  };

  /**
   *   These methods define a interface for the LookAndFeel class of juce.
   *   The Plot class needs a LookAndFeel, that implements these methods.
   *   The default implementation can be seen in, \see scp_lookandfeelmethods.h
   */
  class LookAndFeelMethods : public juce::LookAndFeel_V4 {
   public:
    virtual ~LookAndFeelMethods() = default;

    /** This method draws a frame around the graph area. */
    virtual void drawFrame(juce::Graphics& g,
                           const juce::Rectangle<int> bounds) = 0;

    /** This method draws a single graph line. */
    virtual void drawGraphLine(juce::Graphics& g,
                               const GraphPoints& graph_points,
                               const std::vector<float>& dashed_lengths,
                               const juce::Colour graph_colour) = 0;

    /** This method draws the labels on the x and y axis. */
    virtual void drawGridLabels(juce::Graphics& g,
                                const LabelVector& x_axis_labels,
                                const LabelVector& y_axis_labels) = 0;

    /** This method draws either a vertical or horizontal gridline. */
    virtual void drawGridLine(juce::Graphics& g, const GridLine& grid_line,
                              const bool grid_on) = 0;

    /** This method draws the legend. */
    virtual void drawLegend(juce::Graphics& g, const StringVector& label_texts,
                            const std::vector<juce::Colour>& graph_line_colours,
                            const juce::Rectangle<int>& bounds) = 0;

    /** This method draws the area the user wants to zoom in on. */
    virtual void drawZoomArea(
        juce::Graphics& g, juce::Point<int>& start_coordinates,
        const juce::Point<int>& end_coordinates,
        const juce::Rectangle<int>& graph_bounds) noexcept = 0;

    /** A method to find and get the colour for either a 'ColourIdsGraph'
     * enum.*/
    virtual CONSTEXPR juce::Colour findAndGetColourFromId(
        const ColourIdsGraph colour_id) const noexcept = 0;

    /** A method to find and get the colour for a 'ColourIds' enum. */
    virtual CONSTEXPR juce::Colour findAndGetColourFromId(
        const ColourIds colour_id) const noexcept = 0;

    /** Returns the 'ColourIdsGraph' for a given id.*/
    virtual CONSTEXPR ColourIdsGraph
    getColourFromGraphID(const std::size_t graph_id) const = 0;

    /** Get the graph area bounds, where the graphs and grids are to be drawn.*/
    virtual CONSTEXPR juce::Rectangle<int> getGraphBounds(
        const juce::Rectangle<int> bounds) const noexcept = 0;

    /** Returns the Font used when drawing the grid labels. */
    virtual CONSTEXPR juce::Font getGridLabelFont() const noexcept = 0;

    /** Get the legend position */
    virtual CONSTEXPR juce::Point<int> getLegendPosition(
        const juce::Rectangle<int>& graph_bounds,
        const juce::Rectangle<int>& legend_bounds) const noexcept = 0;

    /** Get the legend bounds */
    virtual CONSTEXPR juce::Rectangle<int> getLegendBounds(
        [[maybe_unused]] const juce::Rectangle<int>& bounds,
        const std::vector<std::string>& label_texts) const noexcept = 0;

    /** Returns the Font used when drawing legends. */
    virtual CONSTEXPR juce::Font getLegendFont() const noexcept = 0;

    /** Get the bounds of the componenet */
    virtual CONSTEXPR juce::Rectangle<int> getPlotBounds(
        juce::Rectangle<int> bounds) const noexcept = 0;

    /** Returns the Font used when drawing the x-, y-axis and title labels. */
    virtual CONSTEXPR juce::Font getXYTitleFont() const noexcept = 0;

    /** Get the x-scaling.*/
    virtual CONSTEXPR Scaling getXScaling() const noexcept = 0;

    /** Get the y-scaling.*/
    virtual CONSTEXPR Scaling getYScaling() const noexcept = 0;

    /** Defines the default colours */
    virtual void setDefaultPlotColours() noexcept = 0;

    /** Updates the x-ticks with auto generated ticks. */
    virtual void updateVerticalGridLineTicksAuto(
        const juce::Rectangle<int>& bounds, const bool tiny_grids,
        const Lim_f x_lim, std::vector<float>& x_ticks) noexcept = 0;

    /** Updates the y-ticks with auto generated ticks. */
    virtual void updateHorizontalGridLineTicksAuto(
        const juce::Rectangle<int>& bounds, const bool tiny_grids,
        const Lim_f y_lim, std::vector<float>& y_ticks) noexcept = 0;

    /** Updates the x-cordinates of the graph points used when drawing a graph
     *  line. It also updates the graph point indices used in
     * 'updateYGraphPoints' */
    virtual void updateXGraphPointsAndIndices(
        const juce::Rectangle<int>& bounds, const Lim_f& x_lim,
        const std::vector<float>& x_data,
        std::vector<std::size_t>& graph_points_indices,
        GraphPoints& graph_points) noexcept = 0;

    /** Updates the y-cordinates of the graph points used when drawing a graph
     * line. */
    virtual void updateYGraphPoints(
        const juce::Rectangle<int>& bounds, const Lim_f& y_lim,
        const std::vector<float>& y_data,
        const std::vector<std::size_t>& graph_points_indices,
        GraphPoints& graph_points) noexcept = 0;

    /** Updates both the vertical and horizontal grid labels. */
    virtual void updateGridLabels(const juce::Rectangle<int>& bounds,
                                  const std::vector<GridLine>& grid_lines,
                                  StringVector& x_label_ticks,
                                  StringVector& y_label_ticks,
                                  LabelVector& x_axis_labels,
                                  LabelVector& y_axis_labels) = 0;

    /** Update the title, x and y axis labels. */
    virtual void updateXYTitleLabels(const juce::Rectangle<int>& bounds,
                                     juce::Label& x_label, juce::Label& y_label,
                                     juce::Label& title_label) = 0;

    /*
        virtual void drawBackground(juce::Graphics &g,
                                juce::Rectangle<int> &bounds) = 0;
                                    */
  };

  //==============================================================================
  /** @internal */
  void resized() override;
  /** @internal */
  void paint(juce::Graphics& g) override;
  /** @internal */
  void parentHierarchyChanged() override;
  /** @internal */
  void lookAndFeelChanged() override;
  /** @internal */
  void mouseDrag(const juce::MouseEvent& event) override;
  /** @internal */
  void mouseDown(const juce::MouseEvent& event) override;
  /** @internal */
  void mouseUp(const juce::MouseEvent& event) override;

 protected:
  /** @internal */
  void initialize();

 private:
  template <Scaling x_scaling, Scaling y_scaling>
  juce::LookAndFeel* castUserLookAndFeel(PlotLookAndFeel* user_look_and_feel);

  void PlotInternal(const std::vector<std::vector<float>>& y_data,
                    const std::vector<std::vector<float>>& x_data = {},
                    ColourVector graph_colours = {});

  void resetLookAndFeel();

  void updateYData(const std::vector<std::vector<float>>& y_data);
  void updateXData(const std::vector<std::vector<float>>& x_data);

  void setAutoXScale();
  void setAutoYScale();

  void updateYLim(const float min, const float max);
  void updateXLim(const float min, const float max);

  void updateGridAndGraphs();

  bool m_x_autoscale = true, m_y_autoscale = true;
  const Scaling m_x_scaling, m_y_scaling;
  scp::Lim_f m_x_lim, m_y_lim, m_x_lim_default, m_y_lim_default;

  GraphLines m_graph_lines;
  std::unique_ptr<Grid> m_grid;
  std::unique_ptr<PlotLabel> m_plot_label;
  std::unique_ptr<Frame> m_frame;
  std::unique_ptr<Legend> m_legend;
  std::unique_ptr<Zoom> m_zoom;

  juce::LookAndFeel* m_lookandfeel;
  std::unique_ptr<LookAndFeelMethods> m_lookandfeel_default;

  juce::ComponentDragger m_comp_dragger;
};

/**
 *  \class SemiLogX
 *  \brief Component for plotting 2-D graph lines where the x-axis is scaled
 *   logarithmic and y-axis linearly.
 */
class SemiLogX : public Plot {
 public:
  SemiLogX() : Plot(Scaling::logarithmic){};
};

/**
 *  \class SemiLogY
 *  \brief Component for plotting 2-D graph lines where the x-axis is scaled
 *   logarithmic and y-axis linearly.
 */
class SemiLogY : public Plot {
 public:
  SemiLogY() : Plot(Scaling::linear, Scaling::logarithmic){};
};

}  // namespace scp
