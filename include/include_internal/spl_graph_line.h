/**
 * @file scp_graph_line.h
 *
 * @brief Componenets for drawing graph lines.
 *
 * @ingroup SimpleCustomPlotInternal
 *
 * @author Frans Rosencrantz
 * Contact: Frans.Rosencrantz@gmail.com
 *
 */

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "scp_datamodels.h"
#include "scp_internal_datamodels.h"

namespace scp {
//==============================================================================
/**
 *  \class GraphLine
 *  \brief A templated class component to draw 2-D lines/marker symbols. This is
 *  a subcomponenet to scp::Plot.
 *
 *  Never use this directley, always use one of the derived classes to choose
 *  the scaling.
 */
template <Scaling x_scaling_T, Scaling y_scaling_T>
class GraphLine : public BaseGraphLine {
 public:
  /** Constructor */
  GraphLine();

  /** @brief Constructor with graph-type argument
   *
   *  Set the type of graph \see scp_plot.h
   *
   *  @param graph_type type of graph.
   */
  GraphLine(const GraphType graph_type);

  /** @brief Set the colour of graph
   *
   *  Set the colour of graph.
   *
   *  @param graph_colour the colour of the graph
   *  @return void.
   */
  void setColour(const juce::Colour graph_colour) override;

  /** @brief Get the colour of graph
   *
   *  Get the colour of graph.
   *
   *  @param graph_colour the colour of the graph
   *  @return void.
   */
  juce::Colour getColour() const noexcept override;

  /** @brief Set the x-limits
   *
   *  Set the limits of x-axis.
   *
   *  @param min minimum value
   *  @param max maximum value
   *  @return void.
   */
  void setXLim(const float min, const float max) override;

  /** @brief Set the y-limits
   *
   *  Set the limits of y-axis.
   *
   *  @param min minimum value
   *  @param max maximum value
   *  @return void.
   */
  void setYLim(const float min, const float max) override;

  /** @brief Set the y-values for the graph-line
   *
   *  Set the y-values.
   *
   *  @param y_values vector of y-values.
   *  @return void.
   */
  void setYValues(const std::vector<float>& y_values) noexcept override;

  /** @brief Set the x-values for the graph-line
   *
   *  Set the x-values.
   *
   *  @param x_values vector of x-values.
   *  @return void.
   */
  void setXValues(const std::vector<float>& x_values) noexcept override;

  /** @brief Get y-values
   *
   *  Get a const reference of the y-values.
   *
   *  @return a const reference of the y-values.
   */
  const std::vector<float>& getYValues() noexcept override;

  /** @brief Get x-values
   *
   *  Get a const reference of the x-values.
   *
   *  @return a const reference of the x-values.
   */
  const std::vector<float>& getXValues() noexcept override;

  /** @brief Get the graph points
   *
   *  Get a const reference of the calculated graph points.
   *
   *  @return const reference of the calculated graph points.
   */
  const GraphPoints& getGraphPoints() noexcept override;

  /** @brief Set a dashed path
   *
   *  Use custom dash-lengths to draw a dashed line. e.g. dashed_lengths = {2,
   *  2, 4, 6} will draw a line of 2 pixels, skip 2 pixels, draw 3 pixels, skip
   *  6 pixels, and then repeat.
   *
   *  @param dashed_lengths
   *  @return void.
   */
  void setDashedPath(
      const std::vector<float>& dashed_lengths) noexcept override;

  void resized() override;
  void paint(juce::Graphics& g) override;
  void lookAndFeelChanged() override;

  void updateYGraphPoints() override;
  void updateXGraphPoints() override;

 private:
  /** @brief An enum to describe the state of the graph.  */
  enum class State {
    Uninitialized,
    XInitialized,
    YInitialized,
    XYInitialized
  } m_state{State::Uninitialized};

  std::vector<float> m_dashed_lengths;
  juce::Colour m_graph_colour;
  GraphType m_graph_type;

  virtual void updateYGraphPointsIntern() noexcept;
  virtual void updateXGraphPointsIntern() noexcept;

  static constexpr auto x_scaling = x_scaling_T;
  static constexpr auto y_scaling = y_scaling_T;

 protected:
  LookAndFeelMethodsBase* m_lookandfeel{nullptr};
  scp::ParamVal<scp::Lim_f> m_x_lim, m_y_lim;

  std::vector<float> m_x_data, m_y_data;
  GraphPoints m_graph_points;
  std::vector<std::size_t> m_graph_point_indices;
};

/**
 *  \class LinearGraphLine
 *  \brief Component to draw 2-D graph line.
 *
 *  Both the x and y axis are linearly scaled.
 */
class LinearGraphLine : public GraphLine<Scaling::linear, Scaling::linear> {
  using GraphLine::GraphLine;
};

/**
 *  \class LogXGraphLine
 *  \brief Component to draw 2-D graph line with logarithmic x axis.
 *
 *  The x axis is logarithmic scaled and y axis is linearly scaled.
 */
class LogXGraphLine : public GraphLine<Scaling::logarithmic, Scaling::linear> {
  using GraphLine::GraphLine;
};

/**
 *  \class LogYGraphLine
 *  \brief Component to draw 2-D graph line with logarithmic y axis.
 *
 *  The x axis is linearly scaled and y axis is logarithmic scaled.
 */
class LogYGraphLine : public GraphLine<Scaling::linear, Scaling::logarithmic> {
  using GraphLine::GraphLine;
};
}  // namespace scp
