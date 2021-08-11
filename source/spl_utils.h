#pragma once
#include "spl_graph_line.h"

namespace scp {
typedef std::vector<std::unique_ptr<GraphLine>> GridLines;

enum scaling { linear = 0, logarithmic = 1 };

template <class T>
struct lim {
  T min;
  T max;
};

typedef lim<float> Lim_f;

template <class num_type>
constexpr std::tuple<num_type, num_type, num_type, num_type> getRectangleMeasures(
    juce::Rectangle<int> graph_area) {
    const auto x = static_cast<num_type>(graph_area.getX());
    const auto y = static_cast<num_type>(graph_area.getY());
    const auto width = static_cast<num_type>(graph_area.getWidth());
    const auto height = static_cast<num_type>(graph_area.getHeight());
    return std::make_tuple(x, y, width, height);
}

template <class T>
class ParamBase {
 public:
  ParamBase(const T param) : m_is_set(false), m_param(param) {}
  ParamBase() = default;
  ~ParamBase() = default;
  operator T() const { return m_param; }

 protected:
  bool m_is_set{false};
  T m_param;
};

template <class T>
class ExplicitBoolOperator : public ParamBase<T> {
 public:
  explicit operator bool() const { return m_is_set; }
};

template <class T>
class ParamVal
    : public std::conditional_t<std::is_same_v<T, bool>, ParamBase<T>,
                                ExplicitBoolOperator<T>> {
 public:
  T& operator=(const T& rhs) {
    if (&m_param == &rhs) return m_param;
    m_param = rhs;
    m_is_set = true;
    return m_param;
  }
};
}  // namespace scp
