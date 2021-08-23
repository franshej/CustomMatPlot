#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "spl_utils.h"

namespace scp {
typedef std::vector<juce::Point<float>> GraphPoints;
typedef uint32_t GraphType;

class LookAndFeelMethodsBase;

struct GraphLine : juce::Component {
 public:
  GraphLine();
  GraphLine(const GraphType graph_type);
  ~GraphLine() = default;

  void setXLim(const float min, const float max);
  void setYLim(const float min, const float max);
  void setYValues(const std::vector<float>& y_values) noexcept;
  void setXValues(const std::vector<float>& x_values) noexcept;
  void setID(const std::size_t id) noexcept;
  std::size_t getID() const noexcept;

  const std::vector<float>& getYValues() noexcept;
  const std::vector<float>& getXValues() noexcept;

  const GraphPoints& getGraphPoints() noexcept;

  void setDashedPath(const std::vector<float>& dashed_lengths) noexcept;

  void resized() override;
  void paint(juce::Graphics& g) override;
  void lookAndFeelChanged() override;

  void calculateYData();
  void calculateXData();

 private:
  juce::Colour getGraphColourFromIndex(const std::size_t index);

  std::vector<float> m_dashed_lengths;
  LookAndFeelMethodsBase* m_lookandfeel{nullptr};
  GraphType m_graph_type;
  std::size_t m_id;

  virtual void calculateYDataIntern(GraphPoints& graph_points) noexcept = 0;
  virtual void calculateXDataIntern(GraphPoints& graph_points) noexcept = 0;

 protected:
  scp::ParamVal<scp::Lim_f> m_x_lim, m_y_lim;

  std::vector<float> m_x_data, m_y_data;
  GraphPoints m_graph_points;
};

struct LinearGraphLine : public GraphLine {
 public:
  using GraphLine::GraphLine;

 private:
  void calculateYDataIntern(GraphPoints& graph_points) noexcept override;
  void calculateXDataIntern(GraphPoints& graph_points) noexcept override;
};

struct LogXGraphLine : public GraphLine {
 public:
  using GraphLine::GraphLine;

 private:
  void calculateYDataIntern(GraphPoints& graph_points) noexcept override;
  void calculateXDataIntern(GraphPoints& graph_points) noexcept override;
};
}  // namespace scp
