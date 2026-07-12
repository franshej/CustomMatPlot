/**
 * @file cmp_plot.h
 * @brief Components for plotting 2-D lines and marker symbols
 * @ingroup CustomMatPlot
 * @details This file contains the Plot class and related components for
 * creating 2D plots with features like line plots, scatter plots, zooming,
 * panning, and customizable axes.
 * @author Frans Rosencrantz
 * @contact Frans.Rosencrantz@gmail.com
 */

#pragma once

#include <memory>
#include <span>

#include "cmp_datamodels.h"
#include "cmp_lookandfeel_base.h"
#include "cmp_version.h"

namespace cmp {

/**
 * @class Plot
 * @brief A component to plot 2-D lines/marker symbols
 * @details This class provides a flexible plotting component with features
 * including:
 *          - Line and scatter plots
 *          - Tracing
 *          - Zooming and panning
 *          - Customizable x/y limits, ticks and tick labels
 *          - Linear and logarithmic axis scaling
 *          - Legend support
 * @see SemiLogX
 * @see SemiLogY
 * @see LogLog
 */
class Plot : public juce::Component {
 public:
  /** Destructor, making sure to set the lookandfeel in all subcomponenets to
   * nullptr. */
  ~Plot();

  /* Constructor **/
  Plot(const Scaling x_scaling = Scaling::linear,
       const Scaling y_scaling = Scaling::linear);

  /**
   * @brief Set the X-limits
   * @param min minimum value
   * @param max maximum value
   */
  void xLim(const float min, const float max);

  /**
   * @brief Set the Y-limits
   * @param min minimum value
   * @param max maximum value
   */
  void yLim(const float min, const float max);

  /**
   * @brief Plot one or more series.
   *
   * Each @ref SeriesData carries its own x, y and optional styling, so the
   * series can no longer fall out of index-alignment the way three parallel
   * vectors could. Leave a series' x empty to auto-generate a 1..N ramp.
   * If a series' attribute has no colour set, 'ColourIdsSeries' from the
   * look and feel is used. Passing an empty list clears the series.
   *
   * @code
   *   plot({ {.x = t, .y = signal, .attribute = {.series_colour = red}},
   *          {.y = samples} });   // second series uses an auto x-ramp
   * @endcode
   *
   * @param series the series to plot @see SeriesData
   */
  void plot(const SeriesDataList &series);

  /**
   * @brief Plot a single series.
   *
   * Convenience overload so a single series need not be wrapped in an extra
   * pair of braces: @code plot({.y = samples}); @endcode
   *
   * @param series the series to plot @see SeriesData
   */
  void plot(const SeriesData &series);

  /**
   * @brief Draw horizontal line(s)
   *
   * Draw horizontal line(s) at the given y-coordinates. Lines can be moved by
   * dragging.
   *
   * @param y_coordinates Y-coordinates where lines will be drawn
   * @param series_attributes Series attributes for the lines
   */
  void plotHorizontalLines(const std::vector<float> &y_coordinates,
                           const SeriesAttributeList &series_attributes = {});

  /**
   * @brief Draw vertical line(s)
   *
   * Draw vertical line(s) at the given x-coordinates. Lines can be moved by
   * dragging.
   *
   * @param x_coordinates X-coordinates where lines will be drawn
   * @param series_attributes Series attributes for the lines
   */
  void plotVerticalLines(const std::vector<float> &x_coordinates,
                         const SeriesAttributeList &series_attributes = {});

  /** @brief Plot, but only update the y-data.
   *
   * This plot function will only update the y-data in the series and only
   * repaint the axes_area, therefore requires less CPU than the 'plot'
   * function. x-data must be set through the 'plot' function before calling
   * this function.
   *
   * @param y_data vector of vectors with the y-values.
   */
  void plotUpdateYOnly(const std::vector<std::vector<float>> &y_data);

  /** @brief Fill the area between two data lines
   *
   * Steps to use:
   * 1. Draw series using plot() or realTimePlot()
   * 2. Call this function to fill area between specified lines
   *
   * @param spread_indices Indices of series to fill between
   * @param fill_area_colours Colors for filled areas (uses ColourIdsSeries if
   * empty)
   */
  void fillBetween(const std::vector<SpreadIndex> &spread_indices,
                   const std::vector<juce::Colour> &fill_area_colours = {});

  /** @brief Set downsampling type.
   *
   * @see cmp::DownsamplingType for the different types.
   * default is DownsamplingType::xy_downsampling.
   * @note The downsampling type will be set to
   * DownsamplingType::no_downsampling if move_point_type >
   * PixelPointMoveType::none.
   *
   * @param downsampling_type the type of downsampling.
   */
  void setDownsamplingType(const DownsamplingType downsampling_type);

  /** @brief Set if it should be possible to move pixel points and in which
   * direction(s).
   *
   * @note The downsampling type will be set to
   * DownsamplingType::no_downsampling if move_point_type >
   * PixelPointMoveType::none.
   * @see cmp::PixelPointMoveType for the different types, default is
   * PixelPointMoveType::none.
   *
   * @param move_points_type the type of pixel point movement.
   * @return void.
   */
  void setMovePointsType(const PixelPointMoveType move_points_type);

  /** @brief Set SeriesChangedCallback.
   *
   * Set a callback function that is triggered when a series data is changed
   * to get the new x/y-data.
   *
   * @see cmp::SeriesChangedCallback for more information.
   * @param series_changed_callback the callback function.
   * @return void.
   */
  void setSeriesDataChangedCallback(
      SeriesChangedCallback series_changed_callback);

  /**
   * @brief Set the text for label on the X-axis
   * @param x_label text to be displayed on the x-axis
   */
  void setXLabel(const std::string &x_label);

  /**
   * @brief Set the text for label on the Y-axis
   * @param y_label text to be displayed on the y-axis
   */
  void setYLabel(const std::string &y_label);

  /**
   * @brief Set x & y-axis scaling
   * @param x_scaling x-axis scaling
   * @param y_scaling y-axis scaling
   * @see cmp::Scaling in cmp:datamodels.h
   */
  void setScaling(Scaling x_scaling, Scaling y_scaling) noexcept;

  /**
   * @brief Set the text for title label
   * @param title text to be displayed as the title
   */
  void setTitle(const std::string &title);

  /**
   * @brief Set trace-point
   *
   * Set a trace-point to the point on a series closest the given
   * coordinate. The tracepoint will be removed if it already exists.
   *
   * @param trace_point_coordinate the coordinate of where the trace-point is
   * wish to be set
   */
  void setTracePoint(const juce::Point<float> &trace_point_coordinate);

  /**
   * @brief Set the text for grid labels on the x-axis
   *
   * Set custom text for the grid labels and overrides the labels made based
   * on the x-data.
   *
   * @param x_labels text for the labels displayed as the x-axis
   */
  void setXTickLabels(const std::vector<std::string> &x_labels);

  /** @brief Set the text for tick labels on the y-axis
   *
   *  Set custom text for the tick labels and overrides the labels made based
   *  the y-data.
   *
   *  @param y_labels text for the labels displayed as the y-axis
   *  @return void.
   */
  void setYTickLabels(const std::vector<std::string> &y_labels);

  /** @brief Set the ticks values
   *
   *  Use custom ticks to draw the grid lines and tick labels where you wanted.
   *
   *  @param x_ticks x-postions of ticks
   *  @return void.
   */
  void setXTicks(const std::vector<float> &x_ticks);

  /** @brief Set the ticks values
   *
   *  Use custom ticks to draw the grid lines and tick labels where you wanted.
   *
   *  @param y_ticks y-postions of ticks
   *  @return void.
   */
  void setYTicks(const std::vector<float> &y_ticks);

  /** @brief Enables grid or tiny grid
   *
   *  Turn on grids or tiny grids. @see GridType in cmp:datamodels.h.
   *
   *  @param grid_type typ of grid to be drawn.
   *  @return void.
   */
  void setGridType(const GridType grid_type);

  /** @brief Clear tracepoints
   *
   * Removes all tracepoints.
   *
   * @return void.
   */
  void clearTracePoints() noexcept;

  /** @brief Set Legend
   *
   *  Set descriptions for each series. The label 'label1..N' will be used if
   *  fewers numbers of series_descriptions are provided than existing numbers
   * of series.
   *
   *  @param series_descriptions description of the series
   *  @return void.
   */
  void setLegend(const std::vector<std::string> &series_descriptions);

  //==============================================================================

  /** @brief This lambda is triggered when a tracepoint value is changed.
   *
   * @param current_plot poiter to this plot.
   * @param previous_trace_point previous tracepoint value.
   * @param new_trace_point the new tracepoint value.
   */
  std::function<void(const juce::Component *current_plot,
                     const juce::Point<float> previous_trace_point,
                     const juce::Point<float> new_trace_point)>
      onTraceValueChange = nullptr;

  //==============================================================================

  /** @brief Color IDs for customizing plot appearance
   * @details These IDs can be used with Component::setColour() or
   * LookAndFeel::setColour() to customize colors of various plot elements.
   * @note All color IDs should be passed to setColour() along with the desired
   * Color value
   * @see Component::setColour
   * @see Component::findColour
   * @see LookAndFeel::setColour
   */
  enum ColourIds : int {
    background_colour,        /** Colour of the background. */
    grid_colour,              /** Colour of the grids. */
    transluent_grid_colour,   /** Colour of the transulent grids. */
    x_grid_label_colour,      /** Colour of the label for each x-grid line. */
    y_grid_label_colour,      /** Colour of the label for each y-grid line. */
    frame_colour,             /** Colour of the frame around the axes area. */
    x_label_colour,           /** Colour of the text on the x-axis. */
    y_label_colour,           /** Colour of the label on the y-axis. */
    title_label_colour,       /** Colour of the title label. */
    trace_background_colour,  /** Colour of the trace background colour. */
    trace_label_frame_colour, /** Colour of the trace label frame. */
    trace_label_colour,       /** Colour of the trace label. */
    trace_point_colour,       /** Colour of the trace point colour. */
    trace_point_frame_colour, /** Colour of the trace point frame colour. */
    legend_label_colour,      /** Colour of the legend label(s). */
    legend_background_colour, /** Colour of the legend background. */
    zoom_frame_colour         /** Colour of the dashed zoom rectangle. */
  };

  /** @brief A set of colour IDs to use to change the colour of each plot
   * line.*/
  enum ColourIdsSeries : int {
    first_series_colour = (1u << 16u), /** Colour of the first series. */
    second_series_colour,              /** Colour of the second series. */
    third_series_colour,               /** Colour of the third series. */
    fourth_series_colour,              /** Colour of the fourth series. */
    fifth_series_colour,               /** Colour of the fifth series. */
    sixth_series_colour                /** Colour of the sixth series. */
  };

  /**
   *   These methods define a interface for the LookAndFeel class of juce.
   *   The Plot class needs a LookAndFeel, that implements these methods.
   *   The dimension-agnostic part of the interface is defined in
   *   \see PlotLookAndFeelBase; this class adds the 2D-specific methods.
   *   The default implementation can be seen in \see cmp_lookandfeelmethods.h
   */
  class LookAndFeelMethods : public PlotLookAndFeelBase {
   public:
    virtual ~LookAndFeelMethods() = default;

    /** This method draws the labels on the x and y axis. */
    virtual void drawGridLabels(juce::Graphics &g,
                                const LabelVector &x_axis_labels,
                                const LabelVector &y_axis_labels) = 0;

    /** This method draws either a vertical or horizontal gridline. */
    virtual void drawGridLine(juce::Graphics &g, const GridLine &grid_line,
                              const GridType grid_type) = 0;

    /** Fill area between two series. */
    virtual void drawSpread(juce::Graphics &g, const Series *first_series,
                            const Series *second_series,
                            const juce::Colour &spread_colour) = 0;

    /** Get position for a single trace point.*/
    virtual CONSTEXPR20 juce::Point<int> getTracePointPositionFrom(
        const juce::Rectangle<int> &axes_bounds, const Lim<float> x_lim,
        const Scaling x_scaling, const Lim<float> y_lim,
        const Scaling y_scaling,
        const juce::Point<float> series_values) const noexcept = 0;

    /** Get distance from left of grid x-labels to right side of axes bound. */
    virtual CONSTEXPR20 int getXGridLabelDistanceFromAxesBound()
        const noexcept = 0;

    /** Get distance from top of grid x-labels to bottom of axes bound. */
    virtual CONSTEXPR20 int getYGridLabelDistanceFromAxesBound(
        const int y_grid_label_width) const noexcept = 0;

    /** Updates the x-ticks with auto generated ticks. */
    virtual void updateVerticalGridLineTicksAuto(
        const juce::Rectangle<int> &bounds, const Lim_f &x_lim,
        const Scaling x_scaling, const GridType grid_type,
        const std::vector<float> &previous_ticks,
        std::vector<float> &x_ticks) noexcept = 0;

    /** Updates the y-ticks with auto generated ticks. */
    virtual void updateHorizontalGridLineTicksAuto(
        const juce::Rectangle<int> &bounds, const Lim_f &y_lim,
        const Scaling y_scaling, const GridType grid_type,
        const std::vector<float> &previous_ticks,
        std::vector<float> &y_ticks) noexcept = 0;

    /** Updates the x-coordinates of the pixel points used when drawing a series
     *  line. The caller owns the sizing of 'pixel_points'; this method only
     *  writes coordinates. */
    virtual void updateXPixelPoints(
        const std::vector<std::size_t> &update_only_these_indices,
        const Scaling x_scaling, const Lim<float> x_lim,
        const juce::Rectangle<int> &axes_bounds,
        const std::vector<float> &x_data,
        std::vector<std::size_t> &pixel_points_indices,
        PixelPoints &pixel_points) noexcept = 0;

    /** Updates the y-coordinates of the pixel points used when drawing a series
     * line. The caller owns the sizing of 'pixel_points'; this method only
     * writes coordinates. */
    virtual void updateYPixelPoints(
        const std::vector<std::size_t> &update_only_these_indices,
        const Scaling y_scaling, const Lim<float> y_lim,
        const juce::Rectangle<int> &axes_bounds,
        const std::vector<float> &y_data,
        const std::vector<std::size_t> &pixel_points_indices,
        PixelPoints &pixel_points) noexcept = 0;

    /** Updates both the vertical and horizontal grid labels. */
    virtual void updateGridLabels(const juce::Rectangle<int> &axes_bounds,
                                  const std::vector<GridLine> &grid_lines,
                                  StringVector &x_label_ticks,
                                  StringVector &y_label_ticks,
                                  LabelVector &x_axis_labels,
                                  LabelVector &y_axis_labels) = 0;

    /** Is x-axis labels above or below the axes area */
    virtual CONSTEXPR20 bool isXAxisLabelsBelowAxesArea() const noexcept = 0;
  };

  //==============================================================================
  /** @internal */
  void resized() override;
  /** @internal */
  void paint(juce::Graphics &g) override;
  /** @internal */
  void parentHierarchyChanged() override;
  /** @internal */
  void lookAndFeelChanged() override;
  /** @internal */
  void mouseDrag(const juce::MouseEvent &event) override;
  /** @internal mouse drag state. */
  MouseDragState m_mouse_drag_state = MouseDragState::none;
  /** @internal */
  void mouseDown(const juce::MouseEvent &event) override;
  /** @internal */
  void mouseUp(const juce::MouseEvent &event) override;
  /** @internal */
  juce::Point<float> getMousePositionRelativeToAxesArea(
      const juce::MouseEvent &event) const;
  /** @internal */
  void modifierKeysChanged(const juce::ModifierKeys &modifiers) override;

 private:
  /** @internal */
  template <bool is_point_data_point = false>
  std::tuple<size_t, const Series *> findNearestPoint(
      juce::Point<float> point, const Series *series = nullptr);
  /** @internal */
  PlotLookAndFeel *getDefaultLookAndFeel();
  /** @internal */
  template <SeriesType t_series_type>
  void addSeriesInternal(std::unique_ptr<Series> &series,
                         const size_t series_index);
  /** @internal */
  void resizeChildrens();
  /** @internal */
  void resetLookAndFeelChildrens(juce::LookAndFeel *lookandfeel = nullptr);
  /** @internal */
  template <SeriesType t_series_type>
  void updateSeriesYData(const std::vector<std::vector<float>> &y_data,
                         const SeriesAttributeList &series_attribute_list);
  /** @internal */
  template <SeriesType t_series_type>
  void updateSeriesXData(const std::vector<std::vector<float>> &x_data);
  /** @internal */
  void setAutoXScale();
  /** @internal */
  void setAutoYScale();
  /** @internal */
  void updateXLim(const Lim_f &new_x_lim);
  /** @internal */
  void updateYLim(const Lim_f &new_y_lim);
  /** @internal */
  void addSelectableTracePoints();
  /** @internal */
  void setTracePointInternal(const juce::Point<float> &trace_point_coordinate,
                             bool is_point_data_point);
  /** @internal Ensure there are exactly 'count' series of the given type,
   * creating any that are missing. */
  template <SeriesType t_series_type>
  void ensureSeriesCount(std::size_t count);
  /** @internal Copy the given series' data into the plot's components (one
   * copy each); shared by the plot(SeriesData) and plot(SeriesDataList)
   * overloads. */
  void plotSeries(std::span<const SeriesData> series);
  /** @internal */
  template <SeriesType t_series_type>
  void plotInternal(const std::vector<std::vector<float>> &y_data,
                    const std::vector<std::vector<float>> &x_data,
                    const SeriesAttributeList &series_attributes,
                    const bool update_y_data_only = false);
  /** @internal */
  std::vector<std::vector<float>> generateXdataRamp(
      const std::vector<std::vector<float>> &y_data);
  /** @internal */
  void syncDownsamplingModeWithMoveType();
  /** @internal */
  void setDownsamplingTypeInternal(const DownsamplingType downsampling_type);

  /** User input related things  */
  /** @internal */
  void mouseHandler(const juce::MouseEvent &event,
                    const UserInputAction user_input_action);
  /** @internal */
  void addOrRemoveTracePoint(const juce::MouseEvent &event);
  /** @internal */
  void resetZoom();
  /** @internal */
  void setStartPosSelectedRegion(const juce::Point<int> &start_position);
  /** @internal */
  void drawSelectedRegion(const juce::Point<int> &end_position);
  /** @internal */
  void zoomOnSelectedRegion();
  /** @internal */
  void moveTracepoint(const juce::MouseEvent &event);
  /** @internal */
  void moveTracepointLabel(const juce::MouseEvent &event);
  /** @internal */
  void moveLegend(const juce::MouseEvent &event);
  /** @internal */
  void selectedTracePointsWithinSelectedArea();
  /** @internal */
  void selectTracePoint(const juce::MouseEvent &event);
  /** @internal */
  void deselectTracePoint(const juce::MouseEvent &event);
  /** @internal */
  void moveSelectedTracePoints(const juce::MouseEvent &event);
  /** @internal */
  void panning(const juce::MouseEvent &event);

  juce::ComponentDragger m_comp_dragger;
  juce::Point<float> m_prev_mouse_position{0.f, 0.f};
  SeriesChangedCallback m_series_changed_callback = nullptr;
  const juce::ModifierKeys *m_modifiers = nullptr;

  /** Common plot parameters. */
  Observable<Scaling> m_x_scaling, m_y_scaling;
  Observable<juce::Rectangle<int>> m_axes_bounds;
  Observable<Lim<float>> m_x_lim, m_y_lim;
  cmp::Lim<float> m_x_lim_start, m_y_lim_start;
  Observable<DownsamplingType> m_downsampling_type;
  Observable<bool> m_notify_components_on_update;

  /** Child components */
  SpreadList m_spread_list;
  std::unique_ptr<SeriesList> m_series;
  std::unique_ptr<Grid> m_grid;
  std::unique_ptr<PlotLabel> m_plot_label;
  std::unique_ptr<Frame> m_frame;
  std::unique_ptr<Legend> m_legend;
  std::unique_ptr<SelectionArea> m_selected_area;
  std::unique_ptr<Trace> m_trace;

  /** Look and feel */
  PlotLookAndFeel *getPlotLookAndFeel();
  std::unique_ptr<PlotLookAndFeel> m_lookandfeel_default;

  /** Friend functions */
  friend const AreLabelsSet areLabelsSet(const Plot *plot) noexcept;
  friend const std::pair<int, int> getMaxGridLabelWidth(
      const Plot *plot) noexcept;

  /** Other variables */
  PixelPointMoveType m_pixel_point_move_type{PixelPointMoveType::none};
  bool m_x_autoscale = true;
  bool m_y_autoscale = true;
  bool m_is_panning_or_zoomed_active = false;
};

/**
 * @class SemiLogX
 * @brief Plot with logarithmic x-axis and linear y-axis scaling
 * @details Convenience class that creates a Plot with logarithmic x-axis
 * scaling. Useful for data that spans multiple orders of magnitude in x.
 */
class SemiLogX : public Plot {
 public:
  SemiLogX() : Plot(Scaling::logarithmic){};
};

/**
 * @class SemiLogY
 * @brief Plot with linear x-axis and logarithmic y-axis scaling
 * @details Convenience class that creates a Plot with logarithmic y-axis
 * scaling. Useful for data that spans multiple orders of magnitude in y.
 */
class SemiLogY : public Plot {
 public:
  SemiLogY() : Plot(Scaling::linear, Scaling::logarithmic){};
};

/**
 * @class LogLog
 * @brief Plot with logarithmic scaling on both axes
 * @details Convenience class that creates a Plot with logarithmic scaling on
 * both axes. Useful for data that spans multiple orders of magnitude in both
 * dimensions.
 */
class LogLog : public Plot {
 public:
  LogLog() : Plot(Scaling::logarithmic, Scaling::logarithmic){};
};

}  // namespace cmp