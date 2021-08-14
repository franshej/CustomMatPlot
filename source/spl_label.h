/**
 * @file spl_label.h
 *
 * @brief Componenets for creating x,y and title labels
 *
 * @ingroup SimpleCustomPlot
 *
 * @author Frans Rosencrantz
 * Contact: Frans.Rosencrantz@gmail.com
 *
 */

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "spl_utils.h"

/**
 * A Parameter struct
 * Containing graphic parameters for the front and the margins from the graph
 * area.
 */
struct LabelGraphicParams {
  /**
   * Font of the x, y and title label
   */
  scp::ParamVal<juce::Font> font;
  /**
   * Margin from the x-axis on graph area of where the x-label will be
   * drawn.
   */
  scp::ParamVal<int> x_margin;
  /**
   * Margin from the y-axis on graph area of where the y-label will be
   * drawn.
   */
  scp::ParamVal<int> y_margin;
  /**
   * Margin from the upper x-axis on graph area of where the title-label will be
   * drawn.
   */
  scp::ParamVal<int> title_margin;
};

/**
 * A class for creating x, y and title labels
 *
 * The idea is to use this componenet to draw the x, y and title labels.
 */
class PlotLabel : public juce::Component {
 public:
  PlotLabel();

  /** @brief Set the x-label text
   *
   *  Set the text that will be display on the x-axis.
   *
   *  @param x_label the text that will be drawn on the x-axis.
   *  @return void.
   */
  void setXLabel(const std::string &x_label);

  /** @brief Set the y-label text
   *
   *  Set the text that will be display on the y-axis.
   *
   *  @param y_label the text that will be drawn on the y-axis.
   *  @return void.
   */
  void setYLabel(const std::string &y_label);

  /** @brief Set the title-label text
   *
   *  Set the text that will be display on the upper x-axis.
   *
   *  @param title_label the text that will be drawn on the upper x-axis
   *  @return void.
   */
  void setTitle(const std::string &title_label);

  /** @brief Set the graphic parameters
   *
   *  Change one or more of the graphic parameters. It's possible to only change
   *  one parameter in the struct without overriding the other parameters.
   *  Example:
   *  LabelGraphicParams params;
   *
   *  params.font = getJuceFont();
   *
   *  // Only the parameter font will be overwritten.
   *  setGraphicParams(params);
   *
   *  @param params label graphic parameter struct
   *  @return void.
   */
  void setGraphicParams(const LabelGraphicParams &params);

  /** @brief Set the bounds of where the graphs are to be drawn
   *
   *  Per deafult The labels will be drawn will a font size margin outside the
   *  graph area.
   *
   *  @param graph_area The area of where the graphs/grids are to be drawn.
   *  @return void.
   */
  void setGraphArea(const juce::Rectangle<int> &graph_area);

  void resized() override;
  void paint(juce::Graphics &g) override;

 private:
  LabelGraphicParams createDefaultGraphicParams();

  LabelGraphicParams m_params;
  juce::Label m_xlabel, m_ylabel, m_title;
  juce::Rectangle<int> m_graph_area;
};