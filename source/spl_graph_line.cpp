#include "spl_graph_line.h"

#include <stdexcept>

#include "spl_plot.h"

namespace scp {

template <Scaling x_scaling_T, Scaling y_scaling_T>
GraphLine<x_scaling_T, y_scaling_T>::GraphLine()
    : m_graph_type(GraphType::graph_line) {}

template <Scaling x_scaling_T, Scaling y_scaling_T>
GraphLine<x_scaling_T, y_scaling_T>::GraphLine(const GraphType graph_type)
    : m_graph_type(graph_type) {}

template <Scaling x_scaling_T, Scaling y_scaling_T>
void GraphLine<x_scaling_T, y_scaling_T>::setColour(
    const juce::Colour graph_colour) {
  m_graph_colour = graph_colour;
}

template <Scaling x_scaling_T, Scaling y_scaling_T>
juce::Colour GraphLine<x_scaling_T, y_scaling_T>::getColour() const noexcept {
  return m_graph_colour;
}

template <Scaling x_scaling_T, Scaling y_scaling_T>
void GraphLine<x_scaling_T, y_scaling_T>::setXLim(const float min,
                                                  const float max) {
  Lim_f x_lim;

  if (min > max)
    throw std::invalid_argument("Min value must be lower than max value.");

  if (abs(max - min) > std::numeric_limits<float>::epsilon()) {
    x_lim.min = min;
    x_lim.max = max;
  } else {
    throw std::invalid_argument(
        "The min and max value of x_values must not be the same.");
  }
  m_x_lim = x_lim;
}

template <Scaling x_scaling_T, Scaling y_scaling_T>
void GraphLine<x_scaling_T, y_scaling_T>::setYLim(const float min,
                                                  const float max) {
  Lim_f y_lim;

  if (min > max)
    throw std::invalid_argument(
        "setYLim min value must be lower than max value.");

  if (abs(max - min) < std::numeric_limits<float>::epsilon()) {
    y_lim.min = 0;
    y_lim.max = std::numeric_limits<float>::max();
  } else {
    y_lim.min = min;
    y_lim.max = max;
  }
  m_y_lim = y_lim;
}

template <Scaling x_scaling_T, Scaling y_scaling_T>
void GraphLine<x_scaling_T, y_scaling_T>::resized() {
  m_state = State::Uninitialized;
};

template <Scaling x_scaling_T, Scaling y_scaling_T>
void GraphLine<x_scaling_T, y_scaling_T>::paint(juce::Graphics& g) {
  if (m_lookandfeel) {
    if (m_graph_points.size() > 2) DBG("Size: " << m_graph_points.size());
    auto lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);
    lnf->drawGraphLine(g, m_graph_points, m_dashed_lengths, m_graph_type,
                       m_graph_colour);
  }
}

template <Scaling x_scaling_T, Scaling y_scaling_T>
void GraphLine<x_scaling_T, y_scaling_T>::lookAndFeelChanged() {
  if (auto* lnf = dynamic_cast<Plot::LookAndFeelMethods*>(&getLookAndFeel())) {
    m_lookandfeel = lnf;
  } else {
    m_lookandfeel = nullptr;
  }
}

template <Scaling x_scaling_T, Scaling y_scaling_T>
void GraphLine<x_scaling_T, y_scaling_T>::setYValues(
    const std::vector<float>& y_data) noexcept {
  if (m_y_data.size() != y_data.size()) m_y_data.resize(y_data.size());
  std::copy(y_data.begin(), y_data.end(), m_y_data.begin());

  m_graph_point_indices.resize(y_data.size());
}

template <Scaling x_scaling_T, Scaling y_scaling_T>
void GraphLine<x_scaling_T, y_scaling_T>::setXValues(
    const std::vector<float>& x_data) noexcept {
  if (m_x_data.size() != x_data.size()) m_x_data.resize(x_data.size());
  std::copy(x_data.begin(), x_data.end(), m_x_data.begin());

  m_graph_point_indices.resize(x_data.size());
}

template <Scaling x_scaling_T, Scaling y_scaling_T>
void GraphLine<x_scaling_T, y_scaling_T>::setDashedPath(
    const std::vector<float>& dashed_lengths) noexcept {
  m_dashed_lengths = dashed_lengths;
}

template <Scaling x_scaling_T, Scaling y_scaling_T>
const std::vector<float>&
GraphLine<x_scaling_T, y_scaling_T>::getYValues() noexcept {
  return m_y_data;
}

template <Scaling x_scaling_T, Scaling y_scaling_T>
const std::vector<float>&
GraphLine<x_scaling_T, y_scaling_T>::getXValues() noexcept {
  return m_x_data;
}

template <Scaling x_scaling_T, Scaling y_scaling_T>
const GraphPoints&
GraphLine<x_scaling_T, y_scaling_T>::getGraphPoints() noexcept {
  return m_graph_points;
}

template <Scaling x_scaling_T, Scaling y_scaling_T>
void GraphLine<x_scaling_T, y_scaling_T>::updateXGraphPoints() {
  if (!m_x_lim) {
    jassert("x_lim must be set to calculate the xdata.");
    return;
  }

  if (m_x_data.empty()) {
    jassert("xdata must be added to calculate");
    return;
  }

  if (m_x_data.size() != m_graph_points.size()) {
    m_graph_points.resize(m_x_data.size());
  }

  updateXGraphPointsIntern();

  m_state = State::XInitialized;
}

template <Scaling x_scaling_T, Scaling y_scaling_T>
void GraphLine<x_scaling_T, y_scaling_T>::updateYGraphPoints() {
  if (!m_y_lim) {
    jassert("y_lim must be set to calculate the ydata.");
    return;
  }

  if (m_y_data.empty()) {
    return;
  }

  if (m_state == State::Uninitialized) {
    jassertfalse;
    return;
  }

  updateYGraphPointsIntern();
}

template <Scaling x_scaling_T, Scaling y_scaling_T>
void GraphLine<x_scaling_T, y_scaling_T>::updateXGraphPointsIntern() noexcept {
  if (m_lookandfeel) {
    auto lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);
    lnf->updateXGraphPointsAndIndices(getBounds(), x_scaling, m_x_lim, m_x_data,
                                      m_graph_point_indices, m_graph_points);
  }
}

template <Scaling x_scaling_T, Scaling y_scaling_T>
void GraphLine<x_scaling_T, y_scaling_T>::updateYGraphPointsIntern() noexcept {
  if (m_lookandfeel) {
    auto lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);
    lnf->updateYGraphPoints(getBounds(), y_scaling, m_y_lim, m_y_data,
                            m_graph_point_indices, m_graph_points);
  }
}

// Explicit template instantiation
template class GraphLine<Scaling::linear, Scaling::linear>;
template class GraphLine<Scaling::logarithmic, Scaling::linear>;
template class GraphLine<Scaling::linear, Scaling::logarithmic>;
template class GraphLine<Scaling::logarithmic, Scaling::logarithmic>;

}  // namespace scp
