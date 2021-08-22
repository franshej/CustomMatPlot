#pragma once
#include "spl_graph_line.h"
#include "spl_plot.h"
#include "spl_utils.h"

namespace scp {
class PlotLookAndFeel : public juce::LookAndFeel_V3,
                        public Plot::LookAndFeelMethods {
 public:
  PlotLookAndFeel() { setDefaultPlotColours(); }

  virtual ~PlotLookAndFeel() override {}

  void setDefaultPlotColours() noexcept override {}

  juce::Rectangle<int> getBounds(juce::Rectangle<int>& bounds) const override {
    auto ret = juce::Rectangle<int>(bounds);
    ret.setX(0);
    ret.setY(0);
    return ret;
  }

  juce::Rectangle<int> getGraphAreaBounds(
      juce::Rectangle<int>& bounds) const override {
    return juce::Rectangle<int>();
  }

  void drawGraphLine(juce::Graphics& g,
                     const std::vector<juce::Point<float>>& graph_points,
                     const std::vector<float>& dashed_lengths) override {
    juce::Path graph_path;
    juce::PathStrokeType p_type(1.0f, juce::PathStrokeType::JointStyle::mitered,
                                juce::PathStrokeType::EndCapStyle::rounded);

    if (graph_points.size() > 1) {
      graph_path.startNewSubPath(graph_points[0]);
      std::for_each(
          graph_points.begin() + 1, graph_points.end(),
          [&](const juce::Point<float>& point) { graph_path.lineTo(point); });

      if (!dashed_lengths.empty()) {
        p_type.createDashedStroke(graph_path, graph_path, dashed_lengths.data(),
                                  dashed_lengths.size());
      }

      g.setColour(juce::Colour(Plot::ColourIds::graph_colour));
      g.strokePath(graph_path, p_type);
    }
  }
};

};  // namespace scp