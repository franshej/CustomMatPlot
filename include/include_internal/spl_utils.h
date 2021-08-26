#pragma once

namespace scp {
/*============================================================================*/

class LookAndFeelMethodsBase;

/*============================================================================*/

class FrameComponent : public juce::Component {
 public:
  FrameComponent(juce::Colour frame_colour) : m_frame_colour(frame_colour){};
  ~FrameComponent() = default;

  void resized() override{};
  void paint(juce::Graphics& g) override {
    g.setColour(m_frame_colour);

    const juce::Rectangle<int> frame = {0, 0, getWidth(), getHeight()};
    g.drawRect(frame);
  };

 private:
  juce::Colour m_frame_colour;
};

/*============================================================================*/

template <class value_type>
constexpr std::tuple<value_type, value_type, value_type, value_type>
getRectangleMeasures(juce::Rectangle<int> grid_area) {
  const auto x = static_cast<value_type>(grid_area.getX());
  const auto y = static_cast<value_type>(grid_area.getY());
  const auto width = static_cast<value_type>(grid_area.getWidth());
  const auto height = static_cast<value_type>(grid_area.getHeight());
  return std::make_tuple(x, y, width, height);
}

/*============================================================================*/

template <class T>
class ParamBase {
 public:
  operator T() const { return m_param; }

 protected:
  bool m_is_set{false};
  T m_param;
};

template <class T>
class ExplicitBoolOperator : public ParamBase<T> {
 public:
  constexpr explicit operator bool() const noexcept { return m_is_set; }
};

template <class T>
class ParamVal
    : public std::conditional_t<std::is_same_v<T, bool>, ParamBase<T>,
                                ExplicitBoolOperator<T>> {
 public:
  ParamVal(const T param) {
    m_is_set = false;
    m_param = param;
  };
  ParamVal(T& param) {
    m_is_set = false;
    m_param = param;
  };
  ParamVal() {
    m_is_set = false;
    m_param = T();
  };
  ~ParamVal() = default;

  T& operator=(const T& rhs) {
    if (&m_param == &rhs) return m_param;
    m_param = rhs;
    m_is_set = true;
    return m_param;
  }
  ParamVal<T> operator=(const ParamVal<T>& rhs) {
    if (this == &rhs) return *this;
    if (rhs) {
      m_param = rhs;
      m_is_set = true;
    }
    return *this;
  }
};
}  // namespace scp
